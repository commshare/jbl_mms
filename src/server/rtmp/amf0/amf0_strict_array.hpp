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

    void setValue(const std::vector<Amf0Data*> & v) {
        datas_ = v;
    }

    int32_t decode(const uint8_t *data, size_t len);

    size_t size() const {
        size_t s = 0;
        s = 1 + 4;// marker + count
        for (size_t i = 0; i < datas_.size(); i++) {
            s += datas_[i]->size();
        }
        return s;
    }
private:
    std::vector<Amf0Data*> datas_;
};
};