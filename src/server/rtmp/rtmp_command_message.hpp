#pragma once
#include "amf0/amf0_object.hpp"
namespace mms {
class RtmpConnectCommandMessage {
public:
    int32_t decode(char *payload, int32_t len) {
        int32_t consumed = 0;
        int pos = 0;
        consumed = transaction_.decode(payload, len);
        if(consumed < 0) {
            return -1;
        }
        pos += consumed;
        payload += consumed;
        len -= consumed;

        consumed = command_object_.decode(payload, len);
        if (consumed < 0) {
            return -2;
        }
        pos += consumed;
        payload += consumed;
        len -= consumed;
        Json::Value root = command_object_.toJson();
        std::cout << root.toStyledString() << std::endl;
        if (len > 0) {
            consumed = optional_user_args_.decode(payload, len);
            if (consumed < 0) {
                return -3;
            }
            pos += consumed;
            payload += consumed;
            len -= consumed;
        }
        
        auto tcUrl = command_object_.getProperty<Amf0String>("tcUrl");
        if (!tcUrl) {
            return -4;
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
        
        return pos;
    }
public:
    Amf0Number transaction_;
    Amf0Object command_object_;
    Amf0Object optional_user_args_;

    std::string tc_url_;
    std::string page_url_;
    std::string swf_url_;
    std::string app_;
    double object_encoding_;
};


};