#include <iostream>

#include "amf0_object.hpp"

#include "amf0_boolean.hpp"
#include "amf0_def.hpp"
#include "amf0_null.hpp"
#include "amf0_number.hpp"
#include "amf0_obj_end.hpp"
#include "amf0_string.hpp"
#include "amf0_undefined.hpp"
#include "amf0_reference.hpp"
#include "amf0_strict_array.hpp"
#include "amf0_date.hpp"

namespace mms {
int32_t Amf0Object::decode(const uint8_t* data, size_t len)
{
    size_t pos = 0;
    Amf0ObjEnd obj_end;
    int32_t consumed = 0;

    if (len < 1) {
        return -1;
    }

    // read key
    AMF0_MARKER_TYPE marker = (AMF0_MARKER_TYPE)data[0];
    if (marker != OBJECT_MARKER) {
        return -2;
    }
    len--;
    data++;
    pos++;

    while (len > 0) {
        consumed = obj_end.decode(data, len);
        if (consumed > 0) { //is obj end
            pos += consumed;
            len -= consumed;
            data += consumed;
            return pos;
        }
        // read key
        if (len < 2) {
            return -3;
        }
        uint16_t key_len = 0;
        char *p = (char*)&key_len;
        p[0] = data[1];
        p[1] = data[0];

        data += 2;
        pos += 2;
        len -= 2;

        if (len < key_len) {
            return -4;
        }

        std::string key;
        key.assign((const char *)data, key_len);

        data += key_len;
        pos += key_len;
        len -= key_len;
        // read data
        marker = (AMF0_MARKER_TYPE)data[0];
        Amf0Data* value = nullptr;
        switch (marker) {
        case NUMBER_MARKER: {
            value = new Amf0Number;
            break;
        }
        case BOOLEAN_MARKER: {
            value = new Amf0Boolean;
            break;
        }
        case STRING_MARKER: {
            value = new Amf0String;
            break;
        }
        case OBJECT_MARKER: {
            value = new Amf0Object;
            break;
        }
        case NULL_MARKER: {
            value = new Amf0Null;
            break;
        }
        case UNDEFINED_MARKER: {
            value = new Amf0Undefined;
            break;
        }
        case REFERENCE_MARKER: {
            value = new Amf0Reference;
        }
        case STRICT_ARRAY_MARKER: {
            value = new Amf0StrictArray;
        }
        case DATE_MARKER: {
            value = new Amf0Date;
        }
        default: {
            return -1;
        }
        }

        if (value != nullptr) {
            int32_t consumed = value->decode(data, len);
            if (consumed < 0) {
                delete value;
                return -1;
            }
            len -= consumed;
            pos += consumed;
            data += consumed;

            auto it = properties_.find(key);
            if (it != properties_.end()) {
                delete it->second;
                it->second = value;
            } else {
                properties_[key] = value;
            }
        }
    }
    return pos;
}

int32_t Amf0Object::encode(uint8_t *buf, size_t len) const {
    uint8_t *data = buf;
    if (len < 1) {
        return -1;
    }
    // marker
    *data = OBJECT_MARKER;
    data++;
    len--;
    
    for (auto & p : properties_) {
        // key
        if (len < 2) {
            return -3;
        }
        *(uint16_t*)data = htons(p.first.size());
        data += 2;
        len -= 2;

        memcpy(data, p.first.data(), p.first.size());
        data += p.first.size();
        len -= p.first.size();

        int32_t consumed = p.second->encode(data, len);
        if (consumed < 0) {
            return -3;
        }
        data += consumed;
        len -= consumed;
    }

    Amf0ObjEnd end;
    if (len < end.size()) {
        return -4;
    }
    int32_t consumed = end.encode(data, len);
    data += consumed;
    len -= consumed;
    return data - buf;
}

Json::Value Amf0Object::toJson() {
    Json::Value root;
    for (auto & p : properties_) {
        switch(p.second->getType()) {
            case NUMBER_MARKER: {
                root[p.first] = ((Amf0Number*)p.second)->getValue();
                break;
            }
            case BOOLEAN_MARKER: {
                root[p.first] = ((Amf0Boolean*)p.second)->getValue();
                break;
            }
            case STRING_MARKER: {
                root[p.first] = ((Amf0String*)p.second)->getValue();
                break;
            }
            case OBJECT_MARKER: {
                root[p.first] = ((Amf0Object*)p.second)->toJson();
                break;
            }
            case NULL_MARKER: {
                root[p.first] = nullptr;
                break;
            }
            case UNDEFINED_MARKER: {
                root[p.first] = "undefined";
                break;
            }
            default : {
                
            }
        }
    }
    return root;
}

};