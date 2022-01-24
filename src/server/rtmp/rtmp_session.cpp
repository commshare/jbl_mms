#include <iostream>
#include "server/rtmp/rtmp_protocol/rtmp_define.hpp"
#include "rtmp_session.hpp"

#include "./amf0/amf0_string.hpp"
#include "./amf0/amf0_number.hpp"
#include "./amf0/amf0_object.hpp"

#include "rtmp_message/command_message/rtmp_connect_command_message.hpp"
#include "rtmp_message/command_message/rtmp_window_ack_size_message.hpp"

namespace mms {
RtmpSession::RtmpSession(RtmpConn *conn):conn_(conn), handshake_(conn), chunk_protocol_(conn) {
}

void RtmpSession::service() {
    if (!handshake_.handshake()) {
        conn_->close(); // 关闭socket
        return;
    }

    int ret = chunk_protocol_.cycleRecvRtmpMessage(std::bind(&RtmpSession::onRecvRtmpMessage, this, std::placeholders::_1));
    if (0 != ret) {
        conn_->close();
    }
}

int32_t RtmpSession::onRecvRtmpMessage(std::shared_ptr<RtmpMessage> rtmp_msg) {
    switch(rtmp_msg->getMessageType()) {
        case RTMP_MESSAGE_TYPE_AMF0_COMMAND: {
            return handleAmf0Command(rtmp_msg);
        }
        case RTMP_MESSAGE_TYPE_USER_CONTROL: {
            return handleUserControlMsg(rtmp_msg);
        }
        case RTMP_MESSAGE_TYPE_ACKNOWLEDGEMENT: {
            return handleAcknowledgement(rtmp_msg);
        }
        default: {

        }
    }
    return 0;
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
    RtmpWindowAckSizeMessage ack(window_ack_size_);
    // std::vector<boost::shared_ptr<RtmpChunk>> chunks = ack.toChunk(out_chunk_size_);

    return true;
}

bool RtmpSession::handleAcknowledgement(std::shared_ptr<RtmpMessage> rtmp_msg) {
    // todo 
    // nothing to do
    return true;
}

bool RtmpSession::handleUserControlMsg(std::shared_ptr<RtmpMessage> rtmp_msg) {
    return true;
}

};