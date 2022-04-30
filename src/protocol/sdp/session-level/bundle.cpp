#include <sstream>
#include <boost/algorithm/string/join.hpp>

#include "bundle.hpp"
#include "base/utils/utils.h"
using namespace mms;
std::string BundleAttr::prefix = "a=group:BUNDLE ";
bool BundleAttr::parse(const std::string & line) {
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
    mids.emplace_back(vs[0]);
    mids.emplace_back(vs[1]);
    return true;
}

std::string BundleAttr::toString() const {
    std::ostringstream oss;
    oss << prefix;
    std::string s = boost::algorithm::join(mids, " ");
    oss << s << std::endl;
    return oss.str();
}