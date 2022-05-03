#include <sstream>
#include <iostream>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string.hpp>

#include "bundle.hpp"
#include "base/utils/utils.h"
using namespace mms;
std::string BundleAttr::prefix = "a=group:BUNDLE ";
bool BundleAttr::parse(const std::string & line) {
    std::string::size_type end_pos = line.rfind("\r");
    if (end_pos == std::string::npos) {
        end_pos = line.size() - 1;
    }
    std::string valid_string = line.substr(prefix.size(), end_pos - prefix.size());
    std::vector<std::string> vs;
    vs = Utils::split(valid_string, " ");
    if (vs.size() != 2) {
        return false;
    }
    mids_.push_back(vs[0]);
    mids_.push_back(vs[1]);
    return true;
}

std::string BundleAttr::toString() const {
    std::ostringstream oss;
    oss << prefix;
    std::cout << "mids.size() =" << mids_[0].size() << " " << mids_[1].size() << std::endl;
    // std::cout << "mid0:" << mids_[0] << "  " << mids_[0].size() << std::endl;
    // std::cout << "mid1:" << mids_[1] << "  " << mids_[1].size() << std::endl;
    // std::string s = boost::algorithm::join(mids_, " ");
    // std::cout << "*************************** s=" << s << std::endl;
    oss << mids_[0] << " " << mids_[1] << std::endl;
    std::cout << "bundle is :" << oss.str();
    return oss.str();
}