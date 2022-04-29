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
            output.emplace_back(strv.substr(first, second - first));

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
            output.emplace_back(strv.substr(first, second - first));

        if (second == std::string::npos)
            break;

        first = second + 1;
    }

    return output;
}

bool Utils::startWith(const std::string_view &str, const std::string_view &prefix)
{
    return str.compare(0, prefix.size(), prefix) == 0;
}

bool Utils::endWith(const std::string_view &str, const std::string_view &suffix)
{
    return str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

uint64_t Utils::rand64()
{
    int64_t rand_data = 0;
    for (int i = 0; i < 8; i++)
    {
        int64_t mask = 0xFF;
        mask = mask << (i * 8);
        int bdata = ((mask) >> (i * 8)) & 0xFF;
        if (bdata > 0)
        {
            int64_t data = rand() % bdata;
            rand_data |= data << (i * 8);
        }
    }
    return rand_data;
}