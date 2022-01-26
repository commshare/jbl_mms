#pragma once
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <unordered_map>

#include "json/json.h"

#include "amf0_def.hpp"
#include "amf0_string.hpp"
#include "amf0_number.hpp"
#include "amf0_boolean.hpp"

namespace mms {
class Amf0Object : public Amf0Data {
public:
    using value_type = std::unordered_map<std::string, Amf0Data*>;
    static const AMF0_MARKER_TYPE marker = OBJECT_MARKER;

    Amf0Object()
        : Amf0Data(OBJECT_MARKER)
    {
    }
    virtual ~Amf0Object()
    {
        for (auto& p : properties_) {
            delete p.second;
        }
        properties_.clear();
    }

    template <typename T>
    boost::optional<typename T::value_type> getProperty(const std::string& key)
    {
        auto it = properties_.find(key);
        if (it == properties_.end() || it->second->getType() != T::marker) {
            return boost::optional<typename T::value_type>();
        }
        return ((T*)it->second)->getValue();
    }

    const std::unordered_map<std::string, Amf0Data*>& getValue()
    {
        return properties_;
    }

    void setValue(std::unordered_map<std::string, Amf0Data*>& v) {
        properties_ = v;
    }

    // int, uint32_t...
    template<typename T,  typename std::enable_if<std::is_integral<T>::value, T>::type = 0>
    void setItemValue(const std::string & k, T v) {
        Amf0Number *d = new Amf0Number;
        d->setValue((double)v);
        auto it = properties_.find(k);
        if (it != properties_.end()) {
            delete it->second;
        }
        properties_[k] = d;
    }
    // float, double ...
    template<typename T, typename std::enable_if<std::is_floating_point<T>::value, T>::type = 0>
    void setItemValue(const std::string & k, T v) {
        Amf0Number *d = new Amf0Number;
        d->setValue(v);
        auto it = properties_.find(k);
        if (it != properties_.end()) {
            delete it->second;
        }
        properties_[k] = d;
    }

    template<typename T>
    void setItemValue(const std::string & k, T v);

    int32_t decode(const uint8_t* data, size_t len);

    size_t size() const {
        size_t s = 0;
        s = 1;// marker + count
        for (auto &p : properties_) {
            s += 2 + p.first.size();// key_len + key_str
            s += p.second->size();
        }
        return s;
    }

    Json::Value toJson();
public:
    std::unordered_map<std::string, Amf0Data*> properties_;
};
};