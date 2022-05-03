#pragma once
#include <string>
// http://mirrors.nju.edu.cn/rfc/inline-errata/rfc5888.html

namespace mms {
struct MidAttr {
public:
    static std::string prefix;
    bool parse(const std::string & line);

    const std::string & getMid() const {
        return mid_;
    }

    void setMid(const std::string & val) {
        mid_ = val;
    }

    std::string toString() const;
public:
    std::string mid_;
};
};