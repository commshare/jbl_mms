#include <sstream>
#include <boost/algorithm/string/join.hpp>
#include "rtpmap.h"
#include "base/utils/utils.h"
using namespace mms;
//a=rtpmap:<payload type> <encoding name>/<clock rate> [/<encoding
//  parameters>]
std::string Rtpmap::prefix = "a=rtpmap:";
bool Rtpmap::parse(const std::string & line) {
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

    payload_type = std::atoi(vs[0].c_str());
    std::vector<std::string> tmp;
    tmp = Utils::split(vs[1], "/");
    if (tmp.size() < 2) {
        return false;
    }
    encoding_name = tmp[0];
    clock_rate = std::atoi(tmp[1].c_str());
    for (size_t i = 2; i < tmp.size(); i++) {
        encoding_params.emplace_back(tmp[i]);
    }
    return true;
}

std::string Rtpmap::toString() const {
    std::ostringstream oss;
    oss << prefix << std::to_string(payload_type) << " " << encoding_name << "/" << clock_rate;
    if (encoding_params.size() > 0) {
        oss << "/" << boost::algorithm::join(encoding_params, "/");
    }
    oss << std::endl;
    return oss.str();
}