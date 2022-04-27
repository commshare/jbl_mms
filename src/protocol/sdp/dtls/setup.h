#pragma once
// https://tools.ietf.org/html/rfc4145#section-4
#include <string>
namespace mms {
struct SetupAttr {
public:
    static std::string prefix;
    bool parse(const std::string & line);
public:
    std::string raw_string;
    std::string valid_string;
    std::string role;
};
};