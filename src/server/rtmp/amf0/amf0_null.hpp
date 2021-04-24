#pragma once
#include "amf0_def.hpp"
namespace mms {
class Amf0Null : public Amf0Data {
public:
    Amf0Null() : type_(NULL_MARKER) {}
    virtual ~Amf0Null() {}
public:
    int32_t decode(char *data, size_t len) {
        return 0;
    }
};
};