#pragma once
#include <string>
// http://mirrors.nju.edu.cn/rfc/inline-errata/rfc5888.html

namespace mms {
struct MidAttr {
public:
    static std::string prefix;
    bool parse(const std::string & line);

    uint32_t getMid() const {
        return mid;
    }

    void setMid(uint32_t val) {
        mid = val;
    }

    std::string toString() const;
public:
    uint32_t mid;
};
};