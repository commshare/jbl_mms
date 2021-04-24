#pragma once
#include <string>
#include "amf0_def.hpp"

namespace mms {
class Amf0String : public Amf0Data {
public:
    Amf0String() : type_(STRING_MARKER){}
    virtual ~Amf0String() {}

    int32_t decode(char *data, size_t len) {
        if(len < 2) {
            return -1;
        }

        uint16_t string_len = 0;
        char *p = (char*)&string_len;
        p[0] = data[1];
        p[1] = data[0];

        if((len - 2) < string_len) {
            return -1;
        }

        memcpy(value_.data(), data + 2, string_len);
        return string_len + 2;
    }

    const std::string & getString() {
        return value_;
    }
public:
    std::string value_;
};

};