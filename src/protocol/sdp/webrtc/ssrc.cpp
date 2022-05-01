#include <sstream>
#include <boost/algorithm/string.hpp>

#include "ssrc.h"
#include "base/utils/utils.h"
using namespace mms;
std::string Ssrc::prefix = "a=ssrc:";
bool Ssrc::parse(const std::string & line) {
    std::string::size_type end_pos = line.rfind("\r");
    if (end_pos == std::string::npos) {
        end_pos = line.size() - 1;
    }
    
    std::string valid_string = line.substr(prefix.size(), end_pos);
    std::vector<std::string> vs;
    vs = Utils::split(valid_string, " ");
    if (vs.size() < 2) {
        return false;
    }

    id = std::atoi(vs[0].c_str());
    if (boost::starts_with(vs[1], "cname:")) {
        cname = vs[1].substr(6);
    } else if (boost::starts_with(vs[1], "mslabel:")) {
        mslabel = vs[1].substr(8);
    } else if (boost::starts_with(vs[1], "label:")) {
        label = vs[1].substr(6);
    } else if (boost::starts_with(vs[1], "msid:")) {
        if (vs.size() < 3) {
            return false;
        }

        mslabel = vs[1].substr(5);
        label = vs[2];
    }

    return true;
}

std::string Ssrc::toString() const {
    std::ostringstream oss;
    oss << prefix << id << " cname:" << cname << std::endl;
    oss << prefix << id << " msid:" << mslabel << " " << label << std::endl;
    oss << prefix << id << " mslabel:" << mslabel << std::endl;
    oss << prefix << id << " label:" << label << std::endl;
    return oss.str();
}