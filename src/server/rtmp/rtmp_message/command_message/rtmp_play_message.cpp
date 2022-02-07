#include "rtmp_play_message.hpp"
using namespace mms;
RtmpPlayMessage::RtmpPlayMessage() {

}

RtmpPlayMessage::~RtmpPlayMessage() {

}

int32_t RtmpPlayMessage::decode(std::shared_ptr<RtmpMessage> rtmp_msg) {
    int32_t consumed = 0;
    int32_t pos = 0;
    const uint8_t *payload = rtmp_msg->payload_;
    int32_t len = rtmp_msg->payload_size_;
    consumed = command_name_.decode(payload, len);
    if (consumed < 0) {
        return -1;
    }
    pos += consumed;
    payload += consumed;
    len -= consumed;

    consumed = transaction_id_.decode(payload, len);
    if(consumed < 0) {
        return -2;
    }
    pos += consumed;
    payload += consumed;
    len -= consumed;

    consumed = command_obj_.decode(payload, len);
    if (consumed < 0) {
        consumed = null_.decode(payload, len);
        if (consumed < 0) {
            return -4;
        }
    }
    pos += consumed;
    payload += consumed;
    len -= consumed;
    
    consumed = stream_name_.decode(payload, len);
    if(consumed < 0) {
        return -5;
    }
    pos += consumed;
    payload += consumed;
    len -= consumed;

    consumed = start_.decode(payload, len);
    if(consumed < 0) {
        return -6;
    }
    pos += consumed;
    payload += consumed;
    len -= consumed;

    consumed = duration_.decode(payload, len);
    if(consumed < 0) {
        return -7;
    }
    pos += consumed;
    payload += consumed;
    len -= consumed;

    consumed = reset_.decode(payload, len);
    if(consumed < 0) {
        return -8;
    }
    pos += consumed;
    payload += consumed;
    len -= consumed;

    return pos;
}

const std::string & RtmpPlayMessage::streamName() const {
    return stream_name_.getValue();
}

std::shared_ptr<RtmpMessage> RtmpPlayMessage::encode() {
    //todo implement this method
    return nullptr;
}
