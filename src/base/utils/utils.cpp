#include "utils.h"
#include <iostream>
using namespace mms;
std::vector<std::string_view> Utils::split(std::string_view strv, std::string_view delims)
{
    std::vector<std::string_view> output;
    size_t first = 0;

    while (first < strv.size())
    {
        const auto second = strv.find_first_of(delims, first);

        if (first != second)
            output.emplace_back(strv.substr(first, second-first));

        if (second == std::string_view::npos)
            break;

        first = second + 1;
    }

    return output;
}

std::vector<std::string> Utils::split(const std::string &strv, const std::string &delims)
{
    std::vector<std::string> output;
    size_t first = 0;

    while (first < strv.size())
    {
        const auto second = strv.find_first_of(delims, first);

        if (first != second)
            output.emplace_back(strv.substr(first, second-first));

        if (second == std::string::npos)
            break;

        first = second + 1;
    }

    return output;
}

bool Utils::startWith(const std::string_view &str, const std::string_view &prefix) {
    return str.compare(0, prefix.size(), prefix) == 0;
}

bool Utils::endWith(const std::string_view &str, const std::string_view &suffix) {
    return str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}