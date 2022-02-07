#pragma once

#include "server/rtmp/amf0/amf0_inc.hpp"
#include "server/rtmp/rtmp_protocol/rtmp_define.hpp"

namespace mms {
class RtmpAccessSampleMessage {
    friend class RtmpSession;
public:
    RtmpAccessSampleMessage(bool video = true, bool audio = true) {
        command_name_.setValue("|RtmpSampleAccess");
        audio_.setValue(audio);
        video_.setValue(video);
    }
public:
    int32_t decode(std::shared_ptr<RtmpMessage> rtmp_msg) {
        return 0;
    }

    std::shared_ptr<RtmpMessage> encode() const {
        size_t s = 0;
        s += command_name_.size();
        s += audio_.size();
        s += video_.size();

        std::shared_ptr<RtmpMessage> rtmp_msg = std::make_shared<RtmpMessage>(s);
        rtmp_msg->chunk_stream_id_ = RTMP_CHUNK_ID_PROTOCOL_CONTROL_MESSAGE;
        rtmp_msg->timestamp_ = 0;
        rtmp_msg->message_type_id_ = RTMP_MESSAGE_TYPE_AMF0_COMMAND;
        rtmp_msg->message_stream_id_ = RTMP_MESSAGE_ID_PROTOCOL_CONTROL;
        // window ack_size
        uint8_t * payload = rtmp_msg->payload_;

        int32_t len = s;
        int32_t consumed = command_name_.encode(payload, len);
        if (consumed < 0) {
            return nullptr;
        }
        payload += consumed;
        len -= consumed;

        consumed = video_.encode(payload, len);
        if (consumed < 0) {
            return nullptr;
        }
        payload += consumed;
        len -= consumed;

        consumed = audio_.encode(payload, len);
        if (consumed < 0) {
            return nullptr;
        }
        payload += consumed;
        len -= consumed;

        rtmp_msg->payload_size_ = s;
        return rtmp_msg;
    }
    
private:
    Amf0String command_name_;
    Amf0Boolean audio_;
    Amf0Boolean video_;
};

};