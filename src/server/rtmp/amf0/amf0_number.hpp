#pragma once
#include "amf0_def.hpp"
namespace mms {
class Amf0Number : public Amf0Data {
public:
    using value_type = double;
    static const AMF0_MARKER_TYPE marker = NUMBER_MARKER;

    Amf0Number() : Amf0Data(NUMBER_MARKER) {}
    virtual ~Amf0Number() {}
public:
    int32_t decode(const uint8_t *data, size_t len){
        int pos = 0;
        if(len < 1) {
            return -1;
        }

        auto marker = data[pos];
        len--;
        pos++;

        if (marker != NUMBER_MARKER) {
            return -2;
        }

        if (len < 8) {
            return -3;
        }
        
        const uint8_t *d = data + pos;
        uint8_t *p = (uint8_t*)&value_;
        p[0] = d[7];
        p[1] = d[6];
        p[2] = d[5];
        p[3] = d[4];
        p[4] = d[3];
        p[5] = d[2];
        p[6] = d[1];
        p[7] = d[0];
        pos += 8;
        len --;
        return pos;
    }

    double getValue() const {
        return value_;
    }

    void setValue(double d) {
        value_ = d;
    }

    size_t size() const {
        return 9;
    }

    double value_;
};
};