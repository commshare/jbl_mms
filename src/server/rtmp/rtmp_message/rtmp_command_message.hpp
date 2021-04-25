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
        std::cout << "connect command decode succeed." << std::endl;
        return pos;
    }
public:
    Amf0Number transaction_;
    Amf0Object command_object_;
    Amf0Object optional_user_args_;
};


};