#pragma once
#include <string_view>
namespace mms {
// 5.10.  Repeat Times ("r=")
//       r=<repeat interval> <active duration> <offsets from start-time>
struct RepeatTimes {
public:
    std::string_view raw_string;
    std::string_view repeat_interval;
    std::string_view active_duration;
    std::string_view offset;
};
};