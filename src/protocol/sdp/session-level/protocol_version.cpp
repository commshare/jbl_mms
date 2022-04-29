
#include <sstream>
#include "protocol_version.hpp"
#include "base/utils/utils.h"
using namespace mms;

std::string ProtocolVersion::prefix = "v=";

bool ProtocolVersion::parse(const std::string & line) {
    std::string::size_type end_pos = line.rfind("\r");
    if (end_pos == std::string::npos) {
        end_pos = line.size() - 1;
    }
    std::string sversion = line.substr(prefix.size(), end_pos);
    version = std::atoi(sversion.c_str());
    return true;
}

void ProtocolVersion::setVersion(int v) {
    version = v;
}

int ProtocolVersion::getVersion() {
    return version;
}

std::string ProtocolVersion::toString() const {
    std::ostringstream oss;
    oss << prefix << std::to_string(version) << std::endl;
    return oss.str();
}