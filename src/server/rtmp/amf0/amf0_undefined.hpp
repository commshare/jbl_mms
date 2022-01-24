#pragma once
#include "amf0_def.hpp"
namespace mms {
class Amf0Undefined : public Amf0Data {
public:
    static const AMF0_MARKER_TYPE marker = UNDEFINED_MARKER;
    Amf0Undefined() : Amf0Data(UNDEFINED_MARKER){}
    virtual ~Amf0Undefined() {}
public:
    int32_t decode(const uint8_t *data, size_t len) {
        return 0;
    }
};
};