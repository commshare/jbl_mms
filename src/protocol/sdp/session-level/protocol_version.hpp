#pragma once
#include <stdint.h>
#include <string_view>

namespace mms {
struct ProtocolVersion {
public:
    int32_t read(uint8_t *data, size_t len) {

    }
public:
    std::string_view version;
};
};