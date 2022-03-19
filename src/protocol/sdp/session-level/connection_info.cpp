#include <boost/algorithm/string.hpp>
#include "base/utils/utils.h"
#include "connection_info.hpp"
#include <iostream>

using namespace mms;
std::string_view ConnectionInfo::prefix = "c=";
bool ConnectionInfo::parse(const std::string_view & line) {
    std::string_view::size_type end_pos = line.rfind("\r");
    if (end_pos == std::string_view::npos) {
        end_pos = line.size() - 1;
    }
    valid_string = line.substr(prefix.size(), end_pos);

    std::vector<std::string_view> vs;
    vs = Utils::split(valid_string, " ");
    if (vs.size() != 3) {
        return false;
    }

    nettype = vs[0];
    addrtype = vs[1];
    std::string_view conn_addr_info = vs[2];
    
    vs = Utils::split(conn_addr_info, "/");
    connection_address = vs[0];
    if (vs.size() >= 2) {
        ttl = vs[1];
    }

    if (vs.size() >= 3) {
        num_of_addr = vs[2];
    }
    
    std::cout << "ConnectionInfo:" << valid_string << std::endl;
    return true;
}