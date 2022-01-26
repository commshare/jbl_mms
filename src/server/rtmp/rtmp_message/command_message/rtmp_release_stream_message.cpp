#include "rtmp_release_stream_message.hpp"
using namespace mms;
RtmpReleaseStreamMessage::RtmpReleaseStreamMessage() {

}

RtmpReleaseStreamMessage::~RtmpReleaseStreamMessage() {

}

int32_t RtmpReleaseStreamMessage::decode(std::shared_ptr<RtmpMessage> rtmp_msg) {
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

    consumed = null_.decode(payload, len);
    if (consumed < 0) {
        return -3;
    }
    pos += consumed;
    payload += consumed;
    len -= consumed;

    consumed = stream_name_.decode(payload, len);
    if (consumed < 0) {
        return -4;
    }
    pos += consumed;
    payload += consumed;
    len -= consumed;
    return pos;
}

std::shared_ptr<RtmpMessage> RtmpReleaseStreamMessage::encode() {
    //todo implement this method
    return nullptr;
}
