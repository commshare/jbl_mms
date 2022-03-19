#pragma once
#include <string_view>
// http://mirrors.nju.edu.cn/rfc/inline-errata/rfc5888.html

namespace mms {
struct MidAttr {
public:
    static std::string_view prefix;
    bool parse(const std::string_view & line);
public:
    std::string_view raw_string;
    std::string_view valid_string;
    std::string_view mid;
};
};