#pragma once
#include <stdint.h>
#include <string>
#include <string_view>

namespace mms {
struct ProtocolVersion {
public:
    static std::string_view prefix;
    bool parse(const std::string_view & line);
    int32_t getVersion();
private:
    std::string_view raw_string;
    std::string_view valid_string;
    std::string_view version;
};
};