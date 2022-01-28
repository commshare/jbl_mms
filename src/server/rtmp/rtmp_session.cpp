#include <iostream>
#include "server/rtmp/rtmp_protocol/rtmp_define.hpp"
#include "rtmp_session.hpp"

#include "amf0/amf0_string.hpp"
#include "amf0/amf0_number.hpp"
#include "amf0/amf0_object.hpp"

#include "rtmp_message/command_message/rtmp_connect_command_message.hpp"
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
#include "rtmp_message/command_message/rtmp_publish_message.hpp"
#include "rtmp_message/command_message/rtmp_onstatus_message.hpp"
#include "rtmp_message/data_message/rtmp_metadata_message.hpp"

#include "core/rtmp_media_source.hpp"

namespace mms {
RtmpSession::RtmpSession(RtmpConn *conn):conn_(conn), handshake_(conn), chunk_protocol_(conn) {
    chunk_protocol_.setOutChunkSize(4096);
}

void RtmpSession::service() {
    if (!handshake_.handshake()) {
        conn_->close(); // 关闭socket
        return;
    }

    int ret = chunk_protocol_.cycleRecvRtmpMessage(std::bind(&RtmpSession::onRecvRtmpMessage, this, std::placeholders::_1));
    if (0 != ret) {
        std::cout << "***************** cycleRecvRtmpMessage end:" << ret << " ***********************" << std::endl;
        conn_->close();
    }
}

bool RtmpSession::onRecvRtmpMessage(std::shared_ptr<RtmpMessage> rtmp_msg) {
    switch(rtmp_msg->getMessageType()) {
        case RTMP_MESSAGE_TYPE_AMF0_COMMAND: {
            return handleAmf0Command(rtmp_msg);
        }
        case RTMP_MESSAGE_TYPE_AMF0_DATA: {
            return handleAmf0Data(rtmp_msg);
        }
        case RTMP_MESSAGE_TYPE_USER_CONTROL: {
            return handleUserControlMsg(rtmp_msg);
        }
        case RTMP_MESSAGE_TYPE_ACKNOWLEDGEMENT: {
            return handleAcknowledgement(rtmp_msg);
        }
        case RTMP_MESSAGE_TYPE_VIDEO: {
            return handleVideoMsg(rtmp_msg);
        }
        case RTMP_MESSAGE_TYPE_AUDIO: {
            return handleAudioMsg(rtmp_msg);
        }
        default: {

        }
    }
    return true;
}

bool RtmpSession::handleAmf0Command(std::shared_ptr<RtmpMessage> rtmp_msg) {
    Amf0String command_name;
    int32_t consumed = command_name.decode(rtmp_msg->payload_, rtmp_msg->payload_size_);
    if (consumed < 0) {
        return false;
    }

    auto name = command_name.getValue();
    if (name == "connect") {
        handleAmf0ConnectCommand(rtmp_msg);
    } else if (name == "releaseStream") {
        handleAmf0ReleaseStreamCommand(rtmp_msg);
    } else if (name == "FCPublish") {
        handleAmf0FCPublishCommand(rtmp_msg);
    } else if (name == "createStream") {
        handleAmf0CreateStreamCommand(rtmp_msg);
    } else if (name == "publish") {
        handleAmf0PublishCommand(rtmp_msg);
    }

    return true;
}

bool RtmpSession::handleAmf0ConnectCommand(std::shared_ptr<RtmpMessage> rtmp_msg) {
    RtmpConnectCommandMessage connect_command;
    auto consumed = connect_command.decode(rtmp_msg);
    if(consumed < 0) {
        return false;
    }
    // send window ack size to client
    RtmpWindowAckSizeMessage window_ack_size_msg(window_ack_size_);
    if (!chunk_protocol_.sendRtmpMessage(window_ack_size_msg)) {
        return false;
    }

    RtmpSetPeerBandwidthMessage set_peer_bandwidth_msg(800000000, LIMIT_TYPE_DYNAMIC);
    if (!chunk_protocol_.sendRtmpMessage(set_peer_bandwidth_msg)) {
        return false;
    }

    RtmpSetChunkSizeMessage set_chunk_size_msg(chunk_protocol_.getOutChunkSize());//todo set out chunk size in conf
    if (!chunk_protocol_.sendRtmpMessage(set_chunk_size_msg)) {
        return false;
    }

    RtmpConnectRespMessage result_msg(connect_command, "_result");
    result_msg.props().setItemValue("fmsVer", "FMS/3,0,1,123");
    result_msg.props().setItemValue("capabilities", 31);

    result_msg.info().setItemValue("level", "status");
    result_msg.info().setItemValue("code", RTMP_RESULT_CONNECT_SUCCESS);
    result_msg.info().setItemValue("description", "Connection succeed.");
    result_msg.info().setItemValue("objEncoding", connect_command.object_encoding_);//todo objencoding需要自己判断
    if (!chunk_protocol_.sendRtmpMessage(result_msg)) {
        return false;
    }
    return true;
}

bool RtmpSession::handleAmf0ReleaseStreamCommand(std::shared_ptr<RtmpMessage> rtmp_msg) {
    RtmpReleaseStreamMessage release_command;
    auto consumed = release_command.decode(rtmp_msg);
    if(consumed < 0) {
        return false;
    }
    
    RtmpReleaseStreamRespMessage result_msg(release_command, "_result");
    if (!chunk_protocol_.sendRtmpMessage(result_msg)) {
        return false;
    }
    return true;
}

bool RtmpSession::handleAmf0FCPublishCommand(std::shared_ptr<RtmpMessage> rtmp_msg) {
    RtmpFCPublishMessage fcpublish_cmd;
    auto consumed = fcpublish_cmd.decode(rtmp_msg);
    if(consumed < 0) {
        return false;
    }
    
    RtmpFCPublishRespMessage result_msg(fcpublish_cmd, "_result");
    if (!chunk_protocol_.sendRtmpMessage(result_msg)) {
        return false;
    }
    return true;
}

bool RtmpSession::handleAmf0CreateStreamCommand(std::shared_ptr<RtmpMessage> rtmp_msg) {
    RtmpCreateStreamMessage create_stream_cmd;
    auto consumed = create_stream_cmd.decode(rtmp_msg);
    if(consumed < 0) {
        return false;
    }

    RtmpCreateStreamRespMessage result_msg(create_stream_cmd, "_result");
    if (!chunk_protocol_.sendRtmpMessage(result_msg)) {
        return false;
    }
    return true;
}

bool RtmpSession::handleAmf0PublishCommand(std::shared_ptr<RtmpMessage> rtmp_msg) {
    RtmpPublishMessage publish_cmd;
    auto consumed = publish_cmd.decode(rtmp_msg);
    if(consumed < 0) {
        return false;
    }

    RtmpOnStatusRespMessage status_msg(publish_cmd.transaction_id_.getValue());
    status_msg.data().setItemValue("level", "status");
    status_msg.data().setItemValue("code", RTMP_STATUS_STREAM_PUBLISH_START);
    status_msg.data().setItemValue("description", "publish start ok.");
    status_msg.data().setItemValue("clientid", "mms");
    if (!chunk_protocol_.sendRtmpMessage(status_msg)) {
        return false;
    }

    media_stream_ = std::unique_ptr<RtmpMediaSource>(new RtmpMediaSource);
    media_stream_->init();
    return true;
}

bool RtmpSession::handleAmf0Data(std::shared_ptr<RtmpMessage> rtmp_msg) {//usually is metadata
    RtmpMetaDataMessage metadata_msg;
    auto consumed = metadata_msg.decode(rtmp_msg);
    if (consumed < 0) {
        return false;
    }
    metadata_ = rtmp_msg;
    return true;
}

bool RtmpSession::handleVideoMsg(std::shared_ptr<RtmpMessage> msg) {
    return media_stream_->processPkt(msg);
}

bool RtmpSession::handleAudioMsg(std::shared_ptr<RtmpMessage> msg) {
    return media_stream_->processPkt(msg);
}

bool RtmpSession::handleAcknowledgement(std::shared_ptr<RtmpMessage> rtmp_msg) {
    // todo 
    // nothing to do
    return true;
}

bool RtmpSession::handleUserControlMsg(std::shared_ptr<RtmpMessage> rtmp_msg) {
    // todo handle user control msg
    return true;
}

};