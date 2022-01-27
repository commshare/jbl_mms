#include "rtmp_metadata_message.hpp"
using namespace mms;
RtmpMetaDataMessage::RtmpMetaDataMessage() {

}

RtmpMetaDataMessage::~RtmpMetaDataMessage() {

}

int32_t RtmpMetaDataMessage::decode(std::shared_ptr<RtmpMessage> rtmp_msg) {
    int32_t consumed = 0;
    int32_t pos = 0;
    const uint8_t *payload = rtmp_msg->payload_;
    int32_t len = rtmp_msg->payload_size_;
    consumed = name_.decode(payload, len);
    if (consumed < 0) {
        return -1;
    }
    pos += consumed;
    payload += consumed;
    len -= consumed;
    std::cout << "****************** name:" << name_.getValue() << " *****************" << std::endl;
    if (name_.getValue() == "@setDataFrame") {
        consumed = name_.decode(payload, len);
        if (consumed < 0) {
            return -2;
        }
        std::cout << "****************** name:" << name_.getValue() << " *****************" << std::endl;
        pos += consumed;
        payload += consumed;
        len -= consumed;
    }

    consumed = metadata_.decode(payload, len);
    if (consumed < 0) {
        return consumed;
    }
    pos += consumed;
    payload += consumed;
    len -= consumed;
    return pos;
}

std::shared_ptr<RtmpMessage> RtmpMetaDataMessage::encode() {
    //todo implement this method
    return nullptr;
}
