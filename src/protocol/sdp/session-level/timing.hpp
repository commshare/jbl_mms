#pragma once
#include <string_view>
namespace mms {
struct Timing {
public:
    std::string_view raw_string;
    std::string_view start_time;
    std::string_view stop_time;
};
};