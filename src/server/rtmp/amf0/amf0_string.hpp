#pragma once
#include <string>
#include <iostream>

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

    const std::string & getValue() {
        return value_;
    }
public:
    std::string value_;
};

};