#pragma once
#include <stdint.h>
#include <string_view>

namespace mms {
struct ProtocolVersion {
public:
    const static std::string prefix;
    int32_t read(const std::string_view & data) {
        std::string_view::size_type end_pos = data.find("\n");
        std::string_view::size_type end = end_pos;
        if (end_pos == std::string_view::npos) {
            return -1;
        }

        if (end_pos >= 1) {
            if (data[end_pos - 1] == '\r') {
                end_pos--;
            }
        }

        if (!data.find(prefix) == 0) {//start_with
            return -2;
        }

        // version = data.substr(sizeof(prefix), end_pos);
        return end + 1;
    }
public:
    std::string_view version;
};
};