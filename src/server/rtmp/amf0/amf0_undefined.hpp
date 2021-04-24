#pragma once
#include "amf0_def.hpp"
namespace mms {
class Amf0Undefined : public Amf0Data {
public:
    Amf0Undefined() : type_(UNDEFINED_MARKER){}
    virtual ~Amf0Undefined() {}
public:
    int32_t decode(char *data, size_t len) {
        return 0;
    }
};
};