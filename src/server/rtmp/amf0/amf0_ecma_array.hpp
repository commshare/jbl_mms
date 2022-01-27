#pragma once
#include <unordered_map>
#include <boost/optional.hpp>
#include "json/json.h"

#include "amf0_def.hpp"
#include "amf0_number.hpp"
#include "amf0_boolean.hpp"
#include "amf0_string.hpp"
#include "amf0_null.hpp"
#include "amf0_undefined.hpp"
#include "amf0_obj_end.hpp"

namespace mms {
class Amf0EcmaArray : public Amf0Data {
public:
    using value_type = std::unordered_map<std::string, Amf0Data*>;
    static const AMF0_MARKER_TYPE marker = ECMA_ARRAY_MARKER;

    Amf0EcmaArray() : Amf0Data(ECMA_ARRAY_MARKER) {}
    virtual ~Amf0EcmaArray() {
        for(auto & p : properties_) {
            delete p.second;
        }
        properties_.clear();
    }

    const std::unordered_map<std::string, Amf0Data*> & getValue() {
        return properties_;
    }
public:
    Json::Value toJson();

    template <typename T>
    boost::optional<typename T::value_type> getProperty(const std::string& key)
    {
        auto it = properties_.find(key);
        if (it == properties_.end() || it->second->getType() != T::marker) {
            return boost::optional<typename T::value_type>();
        }
        return ((T*)it->second)->getValue();
    }

    int32_t decode(const uint8_t *data, size_t len) {
        auto buf_start = data;
        if (len < 1) {
            return -1;
        }
        auto marker = *data;
        if (marker != ECMA_ARRAY_MARKER) {
            return -2;
        }
        data++;
        len--;

        if (len < 4) {
            return -3;
        }

        int32_t count = 0;
        char *p = (char*)&count;
        p[0] = data[3];
        p[1] = data[2];
        p[2] = data[1];
        p[3] = data[0];

        data += 4;
        len -= 4;
        while (count > 0) {
            // read key
            if (len < 2) {
                return -4;
            }
            uint16_t key_len = 0;
            char *p = (char*)&key_len;
            p[0] = data[1];
            p[1] = data[0];

            data += 2;
            len -= 2;

            if (len < key_len) {
                return -5;
            }

            std::string key;
            key.assign((const char *)data, key_len);
            data += key_len;
            len -= key_len;
            // read marker
            if (len < 1) {
                return -6;
            }
        
            AMF0_MARKER_TYPE marker = (AMF0_MARKER_TYPE)(*data);
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
                    return -7;
                }
            }

            if (value != nullptr) {
                int32_t consumed = value->decode(data, len);
                if (consumed < 0) {
                    delete value;
                    return -8;
                }
                len -= consumed;
                data += consumed;
                auto it = properties_.find(key);
                if (it != properties_.end()) {
                    delete it->second;
                }
                properties_[key] = value;
            }
            count--;
        }

        Amf0ObjEnd end;
        auto consumed = end.decode(data, len);
        if (consumed < 0) {
            return -8;
        }
        data += consumed;
        len -= consumed;
        return data - buf_start;
    }

    int32_t encode(uint8_t *buf, size_t len) const {
        return 0;
    }

    std::unordered_map<std::string, Amf0Data*> properties_;
};
};