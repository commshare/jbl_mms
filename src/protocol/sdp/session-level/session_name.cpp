#include <sstream>

#include "session_name.hpp"
#include "base/utils/utils.h"
using namespace mms;
std::string SessionName::prefix = "s=";
bool SessionName::parse(const std::string & line) {
    std::string::size_type end_pos = line.rfind("\r");
    if (end_pos == std::string::npos) {
        end_pos = line.size() - 1;
    }
    session_name = line.substr(prefix.size(), end_pos);
    return true;
}

std::string SessionName::toString() const {
    std::ostringstream oss;
    oss << prefix << session_name << std::endl;
    return oss.str();
}