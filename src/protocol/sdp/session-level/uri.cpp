#include "uri.hpp"
#include <iostream>
#include "base/utils/utils.h"
using namespace mms;
std::string Uri::prefix = "u=";
bool Uri::parse(const std::string & line) {
    std::string::size_type end_pos = line.rfind("\r");
    if (end_pos == std::string::npos) {
        end_pos = line.size() - 1;
    }
    valid_string = line.substr(prefix.size(), end_pos);
    uri = valid_string;
    std::cout << "uri:" << uri << std::endl;
    return true;
}