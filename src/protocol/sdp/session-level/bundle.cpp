#include "bundle.hpp"
#include <iostream>
#include "base/utils/utils.h"
using namespace mms;
std::string_view BundleAttr::prefix = "a=group:BUNDLE ";
bool BundleAttr::parse(const std::string_view & line) {
    std::string_view::size_type end_pos = line.rfind("\r");
    if (end_pos == std::string_view::npos) {
        end_pos = line.size() - 1;
    }
    valid_string = line.substr(prefix.size(), end_pos);
    std::vector<std::string_view> vs;
    vs = Utils::split(valid_string, " ");
    if (vs.size() != 2) {
        return false;
    }
    mids.insert(vs[0]);
    mids.insert(vs[1]);
    return true;
}