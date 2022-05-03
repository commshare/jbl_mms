#include <sstream>
#include <iostream>
#include "mid.h"
#include "base/utils/utils.h"
using namespace mms;
std::string MidAttr::prefix = "a=mid:";
bool MidAttr::parse(const std::string & line) {
    std::string::size_type end_pos = line.rfind("\r");
    if (end_pos == std::string::npos) {
        end_pos = line.size() - 1;
    }
    mid_ = line.substr(prefix.size(), end_pos - prefix.size());
    std::cout << "********************** mid:" << mid_ << " size:" << mid_.size() << " **********************" << std::endl;
    return true;
}

std::string MidAttr::toString() const {
    std::ostringstream oss;
    oss << prefix << mid_ << std::endl;
    return oss.str();
}