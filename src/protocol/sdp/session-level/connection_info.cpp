#include <sstream>

#include "base/utils/utils.h"
#include "connection_info.hpp"

using namespace mms;
std::string ConnectionInfo::prefix = "c=";
bool ConnectionInfo::parse(const std::string & line) {
    std::string::size_type end_pos = line.rfind("\r");
    if (end_pos == std::string::npos) {
        end_pos = line.size() - 1;
    }
    std::string valid_string = line.substr(prefix.size(), end_pos);

    std::vector<std::string> vs;
    vs = Utils::split(valid_string, " ");
    if (vs.size() != 3) {
        return false;
    }

    nettype = vs[0];
    addrtype = vs[1];
    std::string conn_addr_info = vs[2];
    
    vs = Utils::split(conn_addr_info, "/");
    connection_address = vs[0];
    if (vs.size() >= 2) {
        ttl = std::atoi(vs[1].c_str());
    }

    if (vs.size() >= 3) {
        num_of_addr = std::atoi(vs[2].c_str());
    }
    
    return true;
}

std::string ConnectionInfo::toString() const {
    std::ostringstream oss;
    oss << prefix << nettype << " " << addrtype << " " << connection_address;
    if (num_of_addr > 1) {
        oss << "/" << ttl << "/" << num_of_addr;
    }
    oss << std::endl;
    return oss.str();
}