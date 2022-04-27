#pragma once
#include <string>
namespace mms {
struct FingerPrint {
public:
    static std::string prefix;
    bool parse(const std::string & line);
public:
    std::string raw_string;
    std::string valid_string;
    std::string fingerprint;
};
};