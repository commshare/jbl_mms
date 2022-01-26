#include "rtmp_connect_command_message.hpp"
using namespace mms;
RtmpConnectCommandMessage::RtmpConnectCommandMessage() {

}

RtmpConnectCommandMessage::~RtmpConnectCommandMessage() {

}

int32_t RtmpConnectCommandMessage::decode(std::shared_ptr<RtmpMessage> rtmp_msg) {
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

    consumed = command_object_.decode(payload, len);
    if (consumed < 0) {
        return -3;
    }
    pos += consumed;
    payload += consumed;
    len -= consumed;
    
    Json::Value root = command_object_.toJson();
    std::cout << root.toStyledString() << std::endl;
    if (len > 0) {
        consumed = optional_user_args_.decode(payload, len);
        if (consumed < 0) {
            return -4;
        }
        pos += consumed;
        payload += consumed;
        len -= consumed;
    }
    
    auto tcUrl = command_object_.getProperty<Amf0String>("tcUrl");
    if (!tcUrl) {
        return -5;
    }
    tc_url_ = *tcUrl;
    auto pageUrl = command_object_.getProperty<Amf0String>("pageUrl");
    if (pageUrl) {
        page_url_ = *pageUrl;
    }
    
    auto swfUrl = command_object_.getProperty<Amf0String>("swfUrl");
    if (swfUrl) {
        swf_url_ = *swfUrl;
    }

    auto app = command_object_.getProperty<Amf0String>("app");
    if (app) {
        app_ = *app;
    }
    
    auto objectEncoding = command_object_.getProperty<Amf0Number>("objectEncoding");
    if (objectEncoding) {
        object_encoding_ = *objectEncoding;
    }
    std::cout << "tcUrl:" << *tcUrl << std::endl;
    
    return pos;
}

std::shared_ptr<RtmpMessage> RtmpConnectCommandMessage::encode() {
    //todo implement this method
    return nullptr;
}
