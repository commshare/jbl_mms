#include "session_information.hpp"
#include <iostream>
#include "base/utils/utils.h"

using namespace mms;
std::string_view SessionInformation::prefix = "i=";
bool SessionInformation::parse(const std::string_view & line) {
    std::string_view::size_type end_pos = line.rfind("\r");
    if (end_pos == std::string_view::npos) {
        end_pos = line.size() - 1;
    }
    valid_string = line.substr(prefix.size(), end_pos);
    session_information = valid_string;
    std::cout << "session_information:" << session_information << std::endl;
    return true;
}