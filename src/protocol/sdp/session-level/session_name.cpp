#include "session_name.hpp"
#include <iostream>
#include "base/utils/utils.h"
using namespace mms;
std::string SessionName::prefix = "s=";
bool SessionName::parse(const std::string & line) {
    std::string::size_type end_pos = line.rfind("\r");
    if (end_pos == std::string::npos) {
        end_pos = line.size() - 1;
    }
    valid_string = line.substr(prefix.size(), end_pos);
    session_name = valid_string;
    std::cout << "session_name:" << session_name << std::endl;
    return true;
}