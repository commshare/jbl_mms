#pragma once
#include <set>
#include "attribute.hpp"
namespace mms {
struct BundleAttr : public Attribute{
public:
    static std::string prefix;
    virtual bool parse(const std::string & line);
public:
    std::set<std::string> mids;
};
};