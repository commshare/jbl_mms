#include "email.hpp"
#include <iostream>
#include "base/utils/utils.h"
using namespace mms;
std::string EmailAddress::prefix = "e=";
bool EmailAddress::parse(const std::string & line) {
    std::string::size_type end_pos = line.rfind("\r");
    if (end_pos == std::string::npos) {
        end_pos = line.size() - 1;
    }
    valid_string = line.substr(prefix.size(), end_pos);
    address = valid_string;
    std::cout << "email address:" << address << std::endl;
    return true;
}