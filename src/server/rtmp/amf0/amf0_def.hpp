#pragma once
#include <stdint.h>
namespace mms {
enum AMF0_MARKER_TYPE {
    NUMBER_MARKER       =   0x00,
    BOOLEAN_MARKER      =   0x01,
    STRING_MARKER       =   0x02,
    OBJECT_MARKER       =   0x03,
    MOVIECLIP_MARKER    =   0x04,//reserved, not supported 
    NULL_MARKER         =   0x05,
    UNDEFINED_MARKER    =   0x06,
    REFERENCE_MARKER    =   0x07,
    ECMA_ARRAY_MARKER   =   0x08,
    OBJECT_END_MARKER   =   0x09,
    STRICT_ARRAY_MARKER =   0x0a,
    DATE_MARKER         =   0x0B,
    LONG_STRING_MARKER  =   0x0C,
    UNSUPPORTED_MARKER  =   0x0E,//reserved, not supported 
    XML_DOCUMENT_MARKER =   0x0F,
    TYPED_OBJECT_MARKER =   0x10
};

template <typename T>
class Amf0Data {
public:
    virtual int32_t decode(char *data, size_t len) = 0;
    T & getValue() {
        return value_;
    }
protected:
    AMF0_MARKER_TYPE type_;
    T value_;
};

};