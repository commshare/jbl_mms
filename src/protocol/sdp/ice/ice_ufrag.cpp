#include "ice_ufrag.h"
#include <iostream>
#include "base/utils/utils.h"
using namespace mms;
std::string_view IceUfrag::prefix = "a=ice-ufrag:";
bool IceUfrag::parse(const std::string_view & line) {
    std::string_view::size_type end_pos = line.rfind("\r");
    if (end_pos == std::string_view::npos) {
        end_pos = line.size() - 1;
    }
    valid_string = line.substr(prefix.size(), end_pos);
    ufrag = valid_string;
    std::cout << "ufrag:" << ufrag << std::endl;
    return true;
}