#pragma once
#include <string>
#include <iostream>
#include <string.h>
#include <netinet/in.h>
#include "amf0_def.hpp"

namespace mms {
class Amf0String : public Amf0Data {
public:
    using value_type = std::string;
    static const AMF0_MARKER_TYPE marker = STRING_MARKER;
    Amf0String() : Amf0Data(STRING_MARKER){}
    virtual ~Amf0String() {}

    int32_t decode(const uint8_t *data, size_t len) {
        // read marker
        int pos = 0;
        if(len < 1) {
            return -1;
        }

        auto marker = data[pos];
        if (marker != STRING_MARKER) {
            std::cout << "marker is:" << uint32_t(marker) << std::endl;
            return -2;
        }
        len--;
        pos++;
        // read len
        if(len < 2) {
            return -3;
        }

        uint16_t string_len = 0;
        char *p = (char*)&string_len;
        p[0] = data[pos+1];
        p[1] = data[pos];
        pos += 2;
        len -= 2;
        // read data
        if(len < string_len) {
            return -4;
        }
        value_.assign((const char*)(data + pos), string_len);
        pos += string_len;
        len -= string_len;
        return pos;
    }

    int32_t encode(uint8_t *buf, size_t len) const {
        uint8_t *data = buf;
        if (len < 1) {
            return -1;
        }
        // marker
        *data = STRING_MARKER;
        data++;
        len--;
        // len
        if (len < 2) {
            return -2;
        }
        *((uint16_t *)data) = htons(value_.size());
        data += 2;
        len -= 2;
        if (len < value_.size()) {
            return -3;
        }
        // data
        memcpy(data, value_.data(), value_.size());
        data += value_.size();
        len -= value_.size();
        return data - buf;
    }

    const std::string & getValue() {
        return value_;
    }

    void setValue(const std::string & s) {
        value_ = s;
    }

    size_t size() const {
        return 1 + 2 + value_.size();
    }
public:
    std::string value_;
};

};