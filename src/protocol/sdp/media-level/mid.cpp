#include <sstream>
#include "mid.h"
#include "base/utils/utils.h"
using namespace mms;
std::string MidAttr::prefix = "a=mid:";
bool MidAttr::parse(const std::string & line) {
    std::string::size_type end_pos = line.rfind("\r");
    if (end_pos == std::string::npos) {
        end_pos = line.size() - 1;
    }
    std::string smid = line.substr(prefix.size(), end_pos);
    mid = std::atoi(smid.c_str());
    return true;
}

std::string MidAttr::toString() const {
    std::ostringstream oss;
    oss << prefix << mid << std::endl;
    return oss.str();
}