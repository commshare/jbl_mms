#include <boost/algorithm/string.hpp>
#include "base/utils/utils.h"
#include "origin.hpp"
#include <iostream>

using namespace mms;
std::string_view Origin::prefix = "o=";
bool Origin::parse(const std::string_view & line) {
    std::string_view::size_type end_pos = line.rfind("\r");
    if (end_pos == std::string_view::npos) {
        end_pos = line.size() - 1;
    }
    valid_string = line.substr(prefix.size(), end_pos);

    std::vector<std::string_view> vs;
    vs = Utils::split(valid_string, " ");
    if (vs.size() != 6) {
        return false;
    }

    username = vs[0];
    session_id = vs[1];
    session_version = vs[2];
    nettype = vs[3];
    addrtype = vs[4];
    unicast_address = vs[5];
    std::cout << "valid_string:" << valid_string << std::endl;
    return true;
}