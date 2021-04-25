#pragma once
#include "amf0_def.hpp"
namespace mms {
class Amf0Null : public Amf0Data {
public:
    static const AMF0_MARKER_TYPE marker = NULL_MARKER;
    Amf0Null() : Amf0Data(NULL_MARKER) {}
    virtual ~Amf0Null() {}
public:
    int32_t decode(char *data, size_t len) {
        return 0;
    }
};
};