#pragma once
#include <unordered_map>
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include "amf0_def.hpp"
#include "amf0_number.hpp"
#include "amf0_boolean.hpp"
#include "amf0_string.hpp"
#include "amf0_null.hpp"
#include "amf0_undefined.hpp"
#include "amf0_obj_end.hpp"

namespace mms {
class Amf0Object : public Amf0Data {
public:
    using value_type = std::unordered_map<std::string, Amf0Data*>;
    static const AMF0_MARKER_TYPE marker = OBJECT_MARKER;

    Amf0Object() : Amf0Data(OBJECT_MARKER) {}
    virtual ~Amf0Object() {
        for(auto & p : values_map_) {
            delete p.second;
        }
        values_map_.clear();
    }

    template <typename T>
    boost::optional<typename T::value_type> getProperty(const std::string & key) {
        auto it = values_map_.find(key);
        if (it->second->type_ != T::marker) {
            return boost::optional<typename T::value_type>();
        }
        return ((T*)it->second)->getValue();
    }

    const std::unordered_map<std::string, Amf0Data*> & getValue() {
        return values_map_;
    }

    int32_t decode(char *data, size_t len) {
        size_t pos = 0;
        Amf0ObjEnd obj_end;
        int32_t consumed = 0;
        while(1) {    
            consumed = obj_end.decode(data + pos, len);
            if (-1 != consumed) {//is obj end
                pos += consumed;
                len -= consumed;
                return pos;
            }

            if(len < 1) {
                return -1;
            }
            // read key
            AMF0_MARKER_TYPE marker = (AMF0_MARKER_TYPE)data[pos];
            len--;
            pos++;
            if (marker != STRING_MARKER) {
                return -1;
            }
            Amf0String key;
            consumed = key.decode(data + pos, len);
            if (consumed < 0) {
                return -1;
            }
            pos += consumed;
            len -= consumed;
            // read data
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
                auto it = values_map_.find(key.getValue());
                if (it != values_map_.end()) {
                    delete it->second;
                    it->second = value;
                } else {
                    values_map_[key.getValue()] = value;
                }
            }
            
        }
        return len;
    }
public:
    std::unordered_map<std::string, Amf0Data*> values_map_;
};
};