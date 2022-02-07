#pragma once
#include "rtmp_user_ctrl_message.hpp"

namespace mms {
class RtmpStreamBeginMessage : public RtmpUserCtrlMessage {
public:
    RtmpStreamBeginMessage(uint32_t stream_id) : RtmpUserCtrlMessage(RTMP_USER_EVENT_STREAM_BEGIN) {
        stream_id_ = stream_id;
    }

    int32_t decode(std::shared_ptr<RtmpMessage> rtmp_msg) {
        return 0;
    }

    std::shared_ptr<RtmpMessage> encode() const {
        size_t s = 2 + 4;// type + id
        std::shared_ptr<RtmpMessage> rtmp_msg = std::make_shared<RtmpMessage>(s);
        rtmp_msg->chunk_stream_id_ = RTMP_CHUNK_ID_PROTOCOL_CONTROL_MESSAGE;
        rtmp_msg->timestamp_ = 0;
        rtmp_msg->message_type_id_ = RTMP_MESSAGE_TYPE_AMF0_COMMAND;
        rtmp_msg->message_stream_id_ = RTMP_MESSAGE_ID_PROTOCOL_CONTROL;

        uint8_t * payload = rtmp_msg->payload_;
        *(uint16_t *)payload = htons(event_type_);
        *(uint32_t *)(payload + 2) = htonl(stream_id_);
        rtmp_msg->payload_size_ = s;
        return rtmp_msg;
    }
private:
    uint32_t stream_id_;
};
};