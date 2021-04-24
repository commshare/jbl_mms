#pragma once
#include <vector>

#include "amf0_def.hpp"
#include "amf0_number.hpp"
#include "amf0_boolean.hpp"
#include "amf0_string.hpp"
#include "amf0_null.hpp"
#include "amf0_undefined.hpp"

namespace mms {
class Amf0EcmaArray : public Amf0Data {
public:
    Amf0EcmaArray() : type_(ECMA_ARRAY_MARKER) {}
    virtual ~Amf0EcmaArray() {
        for(auto & p : values_) {
            delete p;
        }
        values_.clear();
    }
public:
    int32_t decode(char *data, size_t len) {
        if (len < 4) {
            return -1;
        }

        int32_t count = 0;
        char *p = (char*)&count;
        p[0] = data[3];
        p[1] = data[2];
        p[2] = data[1];
        p[3] = data[0];

        int32_t pos = 4;
        len -= 4;
        while(count > 0) {
            if (len < 1) {
                return -1;
            }

            AMF0_MARKER_TYPE marker = (AMF0_MARKER_TYPE)data[pos];
            len--;
            pos++;
            Amf0Data *value = nullptr;

            switch(marker) {
                case NUMBER_MARKER:{
                    value = new Amf0Number;
                    break;
                }
                case BOOLEAN_MARKER:{
                    value = new Amf0Boolean;
                    break;
                }
                case STRING_MARKER:{
                    value = new Amf0String;
                    break;
                }
                case NULL_MARKER:{
                    value = new Amf0Null;
                    break;
                }
                case UNDEFINED_MARKER:{
                    value = new Amf0Undefined;
                    break;
                }
                default : {
                    return -1;
                }
            }

            if (value != nullptr) {
                int32_t consumed = value->decode(data + pos, len);
                if (consumed < 0) {
                    delete value;
                    return -1;
                }
                len -= consumed;
                pos += consumed;
                values_.emplace_back(value);
            }
        }
    }

    std::vector<Amf0Data*> values_;
};
};