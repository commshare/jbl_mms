#include <iostream>
#include <boost/algorithm/string.hpp>

#include "server/rtmp/rtmp_protocol/rtmp_define.hpp"
#include "rtmp_session.hpp"

#include "amf0/amf0_string.hpp"
#include "amf0/amf0_number.hpp"
#include "amf0/amf0_object.hpp"

#include "rtmp_message/command_message/rtmp_window_ack_size_message.hpp"
#include "rtmp_message/command_message/rtmp_set_peer_bandwidth_message.hpp"
#include "rtmp_message/chunk_message/rtmp_set_chunk_size_message.hpp"
#include "rtmp_message/command_message/rtmp_connect_resp_message.hpp"
#include "rtmp_message/command_message/rtmp_release_stream_message.hpp"
#include "rtmp_message/command_message/rtmp_release_stream_resp_message.hpp"
#include "rtmp_message/command_message/rtmp_fcpublish_message.hpp"
#include "rtmp_message/command_message/rtmp_fcpublish_resp_message.hpp"
#include "rtmp_message/command_message/rtmp_create_stream_message.hpp"
#include "rtmp_message/command_message/rtmp_create_stream_resp_message.hpp"
#include "rtmp_message/command_message/rtmp_onstatus_message.hpp"
#include "rtmp_message/command_message/user_ctrl_message/stream_begin_message.hpp"
#include "rtmp_message/command_message/rtmp_access_sample_message.hpp"

#include "rtmp_message/data_message/rtmp_metadata_message.hpp"

#include "core/rtmp_media_source.hpp"
#include "core/media_manager.hpp"

namespace mms {
RtmpSession::RtmpSession(RtmpConn *conn) : RtmpMediaSource(conn->getWorker()), RtmpMediaSink(conn->getWorker()), conn_(conn), handshake_(conn), chunk_protocol_(conn) {
    chunk_protocol_.setOutChunkSize(4096);
    worker_ = conn->getWorker();
}

void RtmpSession::service(boost::asio::yield_context &yield) {
    if (!handshake_.handshake(yield)) {
        conn_->close(); // 关闭socket
        return;
    }

    int ret = chunk_protocol_.cycleRecvRtmpMessage(std::bind(&RtmpSession::onRecvRtmpMessage, this, std::placeholders::_1, std::placeholders::_2), yield);
    if (0 != ret) {
        conn_->close();
    }
}

bool RtmpSession::onRecvRtmpMessage(std::shared_ptr<RtmpMessage> rtmp_msg, boost::asio::yield_context & yield) {
    switch(rtmp_msg->getMessageType()) {
        case RTMP_MESSAGE_TYPE_AMF0_COMMAND: {
            return handleAmf0Command(rtmp_msg, yield);
        }
        case RTMP_MESSAGE_TYPE_AMF0_DATA: {
            return handleAmf0Data(rtmp_msg, yield);
        }
        case RTMP_MESSAGE_TYPE_USER_CONTROL: {
            return handleUserControlMsg(rtmp_msg, yield);
        }
        case RTMP_MESSAGE_TYPE_ACKNOWLEDGEMENT: {
            return handleAcknowledgement(rtmp_msg, yield);
        }
        case RTMP_MESSAGE_TYPE_VIDEO: {
            return handleVideoMsg(rtmp_msg, yield);
        }
        case RTMP_MESSAGE_TYPE_AUDIO: {
            return handleAudioMsg(rtmp_msg, yield);
        }
        default: {

        }
    }
    return true;
}

bool RtmpSession::handleAmf0Command(std::shared_ptr<RtmpMessage> rtmp_msg, boost::asio::yield_context & yield) {
    Amf0String command_name;
    int32_t consumed = command_name.decode(rtmp_msg->payload_, rtmp_msg->payload_size_);
    if (consumed < 0) {
        return false;
    }

    auto name = command_name.getValue();
    if (name == "connect") {
        return handleAmf0ConnectCommand(rtmp_msg, yield);
    } else if (name == "releaseStream") {
        return handleAmf0ReleaseStreamCommand(rtmp_msg, yield);
    } else if (name == "FCPublish") {
        return handleAmf0FCPublishCommand(rtmp_msg, yield);
    } else if (name == "createStream") {
        return handleAmf0CreateStreamCommand(rtmp_msg, yield);
    } else if (name == "publish") {
        return handleAmf0PublishCommand(rtmp_msg, yield);
    } else if (name == "play") {
        return handleAmf0PlayCommand(rtmp_msg, yield);
    }

    return true;
}

bool RtmpSession::handleAmf0ConnectCommand(std::shared_ptr<RtmpMessage> rtmp_msg, boost::asio::yield_context & yield) {
    RtmpConnectCommandMessage connect_command;
    auto consumed = connect_command.decode(rtmp_msg);
    if(consumed < 0) {
        return false;
    }

    // 获取domain, app, stream_name信息
    if (!parseConnectCmd(connect_command)) {
        return false;
    }

    // send window ack size to client
    RtmpWindowAckSizeMessage window_ack_size_msg(window_ack_size_);
    if (!chunk_protocol_.sendRtmpMessage(window_ack_size_msg, yield)) {
        return false;
    }

    RtmpSetPeerBandwidthMessage set_peer_bandwidth_msg(800000000, LIMIT_TYPE_DYNAMIC);
    if (!chunk_protocol_.sendRtmpMessage(set_peer_bandwidth_msg, yield)) {
        return false;
    }

    RtmpSetChunkSizeMessage set_chunk_size_msg(chunk_protocol_.getOutChunkSize());//todo set out chunk size in conf
    if (!chunk_protocol_.sendRtmpMessage(set_chunk_size_msg, yield)) {
        return false;
    }

    RtmpConnectRespMessage result_msg(connect_command, "_result");
    result_msg.props().setItemValue("fmsVer", "FMS/3,0,1,123");
    result_msg.props().setItemValue("capabilities", 31);

    result_msg.info().setItemValue("level", "status");
    result_msg.info().setItemValue("code", RTMP_RESULT_CONNECT_SUCCESS);
    result_msg.info().setItemValue("description", "Connection succeed.");
    result_msg.info().setItemValue("objEncoding", connect_command.object_encoding_);//todo objencoding需要自己判断
    if (!chunk_protocol_.sendRtmpMessage(result_msg, yield)) {
        return false;
    }
    return true;
}

bool RtmpSession::parseConnectCmd(RtmpConnectCommandMessage & connect_command) {
    {// parse domain
        std::vector<std::string> vs;
        boost::split(vs, connect_command.tc_url_, boost::is_any_of("/"));
        if (vs.size() < 3) {
            return false;
        }

        if (vs[0] != "rtmp:") {
            return false;
        }

        if (vs[1] != "") {
            return false;
        }

        domain_ = vs[2];
        if (domain_.find(":") != std::string::npos) {// 去掉端口号
            std::vector<std::string> tmp;
            boost::split(tmp, domain_, boost::is_any_of(":"));
            if (tmp.size() > 1) {
                domain_ = tmp[0];
            }
        }
    }

    {// parse app
        std::vector<std::string> vs;
        boost::split(vs, connect_command.app_, boost::is_any_of("/"));
        if (vs.size() < 1) {
            return false;
        }

        app_ = vs[0];
        if (vs.size() >= 2) {// 兼容obs推流时，可能将流名写到前面
            // todo 考虑带参数的情况
            stream_name_ = vs[1];
        }
    }

    return true;
}

bool RtmpSession::handleAmf0ReleaseStreamCommand(std::shared_ptr<RtmpMessage> rtmp_msg, boost::asio::yield_context & yield) {
    RtmpReleaseStreamMessage release_command;
    auto consumed = release_command.decode(rtmp_msg);
    if(consumed < 0) {
        return false;
    }
    
    RtmpReleaseStreamRespMessage result_msg(release_command, "_result");
    if (!chunk_protocol_.sendRtmpMessage(result_msg, yield)) {
        return false;
    }
    return true;
}

bool RtmpSession::handleAmf0FCPublishCommand(std::shared_ptr<RtmpMessage> rtmp_msg, boost::asio::yield_context & yield) {
    RtmpFCPublishMessage fcpublish_cmd;
    auto consumed = fcpublish_cmd.decode(rtmp_msg);
    if(consumed < 0) {
        return false;
    }
    
    RtmpFCPublishRespMessage result_msg(fcpublish_cmd, "_result");
    if (!chunk_protocol_.sendRtmpMessage(result_msg, yield)) {
        return false;
    }
    return true;
}

bool RtmpSession::handleAmf0CreateStreamCommand(std::shared_ptr<RtmpMessage> rtmp_msg, boost::asio::yield_context & yield) {
    RtmpCreateStreamMessage create_stream_cmd;
    auto consumed = create_stream_cmd.decode(rtmp_msg);
    if(consumed < 0) {
        return false;
    }

    RtmpCreateStreamRespMessage result_msg(create_stream_cmd, "_result");
    if (!chunk_protocol_.sendRtmpMessage(result_msg, yield)) {
        return false;
    }
    return true;
}

bool RtmpSession::handleAmf0PublishCommand(std::shared_ptr<RtmpMessage> rtmp_msg, boost::asio::yield_context & yield) {
    RtmpPublishMessage publish_cmd;
    auto consumed = publish_cmd.decode(rtmp_msg);
    if(consumed < 0) {
        return false;
    }

    if (!parsePublishCmd(publish_cmd)) {
        return false;
    }

    RtmpOnStatusRespMessage status_msg(publish_cmd.transaction_id_.getValue());
    status_msg.data().setItemValue("level", "status");
    status_msg.data().setItemValue("code", RTMP_STATUS_STREAM_PUBLISH_START);
    status_msg.data().setItemValue("description", "publish start ok.");
    status_msg.data().setItemValue("clientid", "mms");
    if (!chunk_protocol_.sendRtmpMessage(status_msg, yield)) {
        return false;
    }

    return MediaManager::get_mutable_instance().addSource(session_name_, std::dynamic_pointer_cast<MediaSource>(shared_from_this()));
}

bool RtmpSession::parsePublishCmd(RtmpPublishMessage & pub_cmd) {
    auto & stream_name = pub_cmd.streamName();
    if (stream_name_.empty()) {
        stream_name_ = stream_name;
    }

    session_name_ = domain_ + "/" + app_ + "/" + stream_name_;
    return true;
}

bool RtmpSession::handleAmf0PlayCommand(std::shared_ptr<RtmpMessage> rtmp_msg, boost::asio::yield_context & yield) {
    RtmpPlayMessage play_cmd;
    auto consumed = play_cmd.decode(rtmp_msg);
    if(consumed < 0) {
        return false;
    }

    if (!parsePlayCmd(play_cmd)) {
        return false;
    }

    RtmpStreamBeginMessage stream_begin_msg(1);// 只用1这个stream_id
    if (!chunk_protocol_.sendRtmpMessage(stream_begin_msg, yield)) {
        return false;
    }

    RtmpOnStatusRespMessage status_msg(play_cmd.transaction_id_.getValue());
    status_msg.data().setItemValue("level", "status");
    status_msg.data().setItemValue("code", RTMP_STATUS_STREAM_PLAY_START);
    status_msg.data().setItemValue("description", "play start ok.");
    status_msg.data().setItemValue("clientid", "mms");
    if (!chunk_protocol_.sendRtmpMessage(status_msg, yield)) {
        return false;
    }

    RtmpAccessSampleMessage access_sample_msg;
    if (!chunk_protocol_.sendRtmpMessage(access_sample_msg, yield)) {
        return false;
    }
    // todo: how to record 404 error to log.
    auto s = MediaManager::get_mutable_instance().getSource(session_name_);
    if (!s) {
        return false;
    }

    return s->addMediaSink(std::dynamic_pointer_cast<MediaSink>(shared_from_this()));
}

bool RtmpSession::parsePlayCmd(RtmpPlayMessage & play_cmd) {
    auto & stream_name = play_cmd.streamName();
    if (stream_name_.empty()) {
        stream_name_ = stream_name;
    }

    session_name_ = domain_ + "/" + app_ + "/" + stream_name_;
    return true;
}

bool RtmpSession::handleAmf0Data(std::shared_ptr<RtmpMessage> rtmp_msg, boost::asio::yield_context & yield) {//usually is metadata
    std::shared_ptr<RtmpMetaDataMessage> metadata = std::make_shared<RtmpMetaDataMessage>();
    auto consumed = metadata->decode(rtmp_msg);
    if (consumed < 0) {
        return false;
    }

    RtmpMediaSource::onMetadata(metadata);
    return true;
}

bool RtmpSession::handleVideoMsg(std::shared_ptr<RtmpMessage> msg, boost::asio::yield_context & yield) {
    return RtmpMediaSource::onVideoPacket(msg);
}

bool RtmpSession::handleAudioMsg(std::shared_ptr<RtmpMessage> msg, boost::asio::yield_context & yield) {
    return RtmpMediaSource::onAudioPacket(msg);
}

bool RtmpSession::handleAcknowledgement(std::shared_ptr<RtmpMessage> rtmp_msg, boost::asio::yield_context & yield) {
    // todo 
    // nothing to do
    return true;
}

bool RtmpSession::handleUserControlMsg(std::shared_ptr<RtmpMessage> rtmp_msg, boost::asio::yield_context & yield) {
    // todo handle user control msg
    return true;
}

bool RtmpSession::sendRtmpMessage(std::shared_ptr<RtmpMessage> msg,boost::asio::yield_context & yield) {
    return chunk_protocol_._sendRtmpMessage(msg, yield);
}

bool RtmpSession::onRtmpPacket(std::shared_ptr<RtmpMessage> msg, boost::asio::yield_context & yield) {
    auto ret = chunk_protocol_._sendRtmpMessage(msg, yield, true);
    if (!ret) {
        conn_->close();
    }
    return ret;
}

void RtmpSession::close() {

}

};