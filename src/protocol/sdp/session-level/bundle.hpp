#pragma once
#include <set>
#include "attribute.hpp"
namespace mms {
struct BundleAttr : public Attribute{
public:
    static std::string_view prefix;
    virtual bool parse(const std::string_view & line);
public:
    std::set<std::string_view> mids;
};
};