#pragma once
// https://tools.ietf.org/html/rfc4145#section-4
#include <string>
namespace mms {
struct TlsIdAttr {
public:
    static std::string prefix;
    bool parse(const std::string & line);
    std::string toString() const;
    const std::string & getId() const {
        return id;
    }

    void setId(const std::string &val) {
        id = val;
    }
public:
    std::string id;
};
};