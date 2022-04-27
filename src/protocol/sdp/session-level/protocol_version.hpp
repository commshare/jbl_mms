#pragma once
#include <stdint.h>
#include <string>
#include <string>

namespace mms {
struct ProtocolVersion {
public:
    static std::string prefix;
    bool parse(const std::string & line);
    int32_t getVersion();
private:
    std::string raw_string;
    std::string valid_string;
    std::string version;
};
};