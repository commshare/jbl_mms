#pragma once

#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <unordered_map>

#include "json/json.h"

#include "amf0_def.hpp"

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

    int32_t decode(const uint8_t* data, size_t len);

    Json::Value toJson();
public:
    std::unordered_map<std::string, Amf0Data*> properties_;
};
};