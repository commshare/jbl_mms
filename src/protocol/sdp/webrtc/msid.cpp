#include <boost/algorithm/string.hpp>
#include <iostream>

#include "msid.hpp"
#include "base/utils/utils.h"
using namespace mms;
std::string Msid::prefix = "a=msid:";
bool Msid::parse(const std::string & line) {
    std::string::size_type end_pos = line.rfind("\r");
    if (end_pos == std::string::npos) {
        end_pos = line.size() - 1;
    }
    std::string valid_string = line.substr(prefix.size(), end_pos);
    std::vector<std::string> vs;
    vs = Utils::split(valid_string, " ");
    if (vs.size() != 2) {
        return false;
    }

    msid = vs[0];
    appdata = vs[1];
    return true;
}