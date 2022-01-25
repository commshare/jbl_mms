#pragma once
#include <stdint.h>
#include "server/rtmp/rtmp_protocol/rtmp_define.hpp"

namespace mms {
class RtmpWindowAckSizeMessage {
public:
    RtmpWindowAckSizeMessage(uint32_t size) : ack_window_size_(size) {

    }

    uint32_t size() {
        return 0;
    }

    int32_t decode(std::shared_ptr<RtmpMessage> rtmp_msg) {
        uint8_t * payload = rtmp_msg->payload_;
        int32_t len = rtmp_msg->payload_size_;
        if (len < 4) {
            return -1;
        }

        int32_t s = 0;
        char *p = (char*)&s;
        p[0] = payload[3];
        p[1] = payload[2];
        p[2] = payload[1];
        p[3] = payload[0];
        ack_window_size_ = s;
        return 4;
    }

    std::shared_ptr<RtmpMessage> encode() const {
        std::shared_ptr<RtmpMessage> rtmp_msg = std::make_shared<RtmpMessage>(sizeof(ack_window_size_));
        rtmp_msg->chunk_stream_id_ = RTMP_CHUNK_ID_PROTOCOL_CONTROL_MESSAGE;
        rtmp_msg->timestamp_ = 0;
        rtmp_msg->message_type_id_ = RTMP_MESSAGE_TYPE_WINDOW_ACK_SIZE;
        rtmp_msg->message_stream_id_ = RTMP_MESSAGE_ID_PROTOCOL_CONTROL;
        char *p = (char *)&ack_window_size_;
        uint8_t * payload = rtmp_msg->payload_;
        payload[3] = p[0];
        payload[2] = p[1];
        payload[1] = p[2];
        payload[0] = p[3];
        rtmp_msg->payload_size_ = 4;
        return rtmp_msg;
    }
private:
    uint32_t ack_window_size_ = 0;
};
};