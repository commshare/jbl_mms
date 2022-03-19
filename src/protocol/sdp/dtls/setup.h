#pragma once
// https://tools.ietf.org/html/rfc4145#section-4
#include <string_view>
namespace mms {
struct SetupAttr {
public:
    static std::string_view prefix;
    bool parse(const std::string_view & line);
public:
    std::string_view raw_string;
    std::string_view valid_string;
    std::string_view role;
};
};