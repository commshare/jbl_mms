#pragma once
#include <string>
// http://mirrors.nju.edu.cn/rfc/inline-errata/rfc5888.html

namespace mms {
struct MidAttr {
public:
    static std::string prefix;
    bool parse(const std::string & line);
public:
    std::string raw_string;
    std::string valid_string;
    std::string mid;
};
};