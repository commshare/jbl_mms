
#include "protocol_version.hpp"
#include <iostream>
#include "base/utils/utils.h"
using namespace mms;

std::string ProtocolVersion::prefix = "v=";

bool ProtocolVersion::parse(const std::string & line) {
    std::string::size_type end_pos = line.rfind("\r");
    if (end_pos == std::string::npos) {
        end_pos = line.size() - 1;
    }
    valid_string = line.substr(prefix.size(), end_pos);
    version = valid_string;
    std::cout << "version:" << version << std::endl;
    return true;
}