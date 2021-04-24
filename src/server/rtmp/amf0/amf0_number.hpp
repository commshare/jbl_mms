#pragma once
#include "amf0_def.hpp"
namespace mms {
class Amf0Number : public Amf0Data {
public:
    Amf0Number() : type_(NUMBER_MARKER) {}
    virtual ~Amf0Number() {}
public:
    int32_t decode(char *data, size_t len){
        if(len <= 8) {
            return -1;
        }
        char *p = (char*)&value_;
        p[0] = data[7];
        p[1] = data[6];
        p[2] = data[5];
        p[3] = data[4];
        p[4] = data[3];
        p[5] = data[2];
        p[6] = data[1];
        p[7] = data[0];
        return 8;
    }

    double value_;
};
};