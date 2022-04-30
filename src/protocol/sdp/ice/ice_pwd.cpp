#include "ice_pwd.h"
#include <sstream>
#include "base/utils/utils.h"
using namespace mms;
std::string IcePwd::prefix = "a=ice-pwd:";
bool IcePwd::parse(const std::string & line) {
    std::string::size_type end_pos = line.rfind("\r");
    if (end_pos == std::string::npos) {
        end_pos = line.size() - 1;
    }
    pwd = line.substr(prefix.size(), end_pos);
    return true;
}

std::string IcePwd::toString() const {
    std::ostringstream oss;
    oss << prefix << pwd << std::endl;
    return oss.str();
}