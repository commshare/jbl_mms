#include "fingerprint.h"
#include <sstream>
#include "base/utils/utils.h"
using namespace mms;
std::string FingerPrint::prefix = "a=setup:";
bool FingerPrint::parse(const std::string & line) {
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
    hash_name = vs[0];
    hash_val = vs[1];
    return true;
}

std::string FingerPrint::toString() const {
    std::ostringstream oss;
    oss << prefix << hash_name << " " << hash_val << std::endl;
    return oss.str();
}