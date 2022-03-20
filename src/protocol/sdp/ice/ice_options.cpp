#include "ice_options.h"
#include <iostream>
#include "base/utils/utils.h"
using namespace mms;
std::string_view IceOption::prefix = "a=ice-options:";
bool IceOption::parse(const std::string_view & line) {
    std::string_view::size_type end_pos = line.rfind("\r");
    if (end_pos == std::string_view::npos) {
        end_pos = line.size() - 1;
    }
    valid_string = line.substr(prefix.size(), end_pos);
    option = valid_string;
    std::cout << "option:" << option << std::endl;
    return true;
}