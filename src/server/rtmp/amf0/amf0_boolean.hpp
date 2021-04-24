#pragma once
#include "amf0_def.hpp"
namespace mms {
class Amf0Boolean : public Amf0Data<bool> {
public:
    Amf0Boolean() : type_(BOOLEAN_MARKER) {}
    virtual ~Amf0Boolean() {}
    int32_t decode(char *data, size_t len) {
        if(len < 1) {
            return -1;
        }
        
        if (data[0] > 0) {
            value_ = true;
        } else {
            value_ = false;
        }
        return 1;
    }
};
};