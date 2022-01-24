#pragma once
#include <string>
#include <vector>

#include "amf0_def.hpp"
namespace mms {
class Amf0StrictArray : public Amf0Data {
public:
    using value_type = std::vector<Amf0Data*>;
    static const AMF0_MARKER_TYPE marker = STRICT_ARRAY_MARKER;

public:
    Amf0StrictArray()
        : Amf0Data(STRICT_ARRAY_MARKER)
    {
    }

    const std::vector<Amf0Data*>& getValue()
    {
        return datas_;
    }

    int32_t decode(const uint8_t *data, size_t len);
private:
    std::vector<Amf0Data*> datas_;
};
};