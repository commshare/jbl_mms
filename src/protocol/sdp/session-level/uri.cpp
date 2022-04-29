#include <sstream>
#include "uri.hpp"
#include "base/utils/utils.h"
using namespace mms;
std::string Uri::prefix = "u=";
bool Uri::parse(const std::string & line) {
    std::string::size_type end_pos = line.rfind("\r");
    if (end_pos == std::string::npos) {
        end_pos = line.size() - 1;
    }
    uri = line.substr(prefix.size(), end_pos);
    return true;
}

std::string Uri::toString() const {
    std::ostringstream oss;
    oss << prefix << uri << std::endl;
    return oss.str();
}