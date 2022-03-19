#pragma once
#include <string_view>
namespace mms {
struct Msid {
public:
    static std::string_view prefix;
    virtual bool parse(const std::string_view & line);
public:
    std::string_view raw_string;
    std::string_view valid_string;
    std::string_view msid;
    std::string_view appdata;
};
};