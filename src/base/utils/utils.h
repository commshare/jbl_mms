#pragma once
#include <vector>
#include <string_view>
namespace mms {
class Utils {
public:
    static std::vector<std::string_view> split(std::string_view strv, std::string_view delims = " ");
    static std::vector<std::string> split(const std::string &strv, const std::string &delims);
    static bool startWith(const std::string_view &str, const std::string_view &prefix);
    static bool endWith(const std::string_view &str, const std::string_view &suffix);
};
};