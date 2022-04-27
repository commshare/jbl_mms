#pragma once
#include <string>
namespace mms {
struct Msid {
public:
    static std::string prefix;
    virtual bool parse(const std::string & line);
public:
    std::string raw_string;
    std::string valid_string;
    std::string msid;
    std::string appdata;
};
};