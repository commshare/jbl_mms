#pragma once
#include <stdint.h>
#include <string>
#include <string_view>

namespace mms {
class H264NALU {
public:
    uint8_t nalu_header;//F + NRI + TYPE
    std::string rbsp;
};

};