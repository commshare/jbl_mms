#include <exception>
#include <sstream>
#include "base/utils/utils.h"
#include "origin.hpp"

using namespace mms;
std::string Origin::prefix = "o=";
bool Origin::parse(const std::string & line) {
    std::string::size_type end_pos = line.rfind("\r");
    if (end_pos == std::string::npos) {
        end_pos = line.size() - 1;
    }
    std::string valid_string = line.substr(prefix.size(), end_pos);

    std::vector<std::string> vs;
    vs = Utils::split(valid_string, " ");
    if (vs.size() != 6) {
        return false;
    }

    try {
        username = vs[0];
        session_id = std::atoll(vs[1].c_str());
        session_version = std::atoi(vs[2].c_str());
        nettype = vs[3];
        addrtype = vs[4];
        unicast_address = vs[5];
    } catch(std::exception & e) {
        return false;
    }
    return true;
}

std::string Origin::toString() const {
    std::string line;
    std::ostringstream oss;
    oss << prefix << username << " " << session_id << " " << session_version << " " << nettype << " " << addrtype << " " << unicast_address << std::endl;
    return oss.str();
}