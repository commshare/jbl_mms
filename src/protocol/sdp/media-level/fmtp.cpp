#include <sstream>
#include <boost/algorithm/string.hpp>
#include "base/utils/utils.h"

#include "fmtp.h"
using namespace mms;
std::string Fmtp::prefix = "a=fmtp:";
std::string Fmtp::empty_str = "";

bool Fmtp::isMyPrefix(const std::string & line)
{
    if (boost::starts_with(line, prefix)) {
        return true;
    } 
    return false;
}

bool Fmtp::parse(const std::string & line)
{
    std::string::size_type end_pos = line.rfind("\r");
    if (end_pos == std::string::npos) {
        end_pos = line.size() - 1;
    }
    std::string valid_string = line.substr(prefix.size(), end_pos - prefix.size());
    std::vector<std::string> vs;
    boost::split(vs, valid_string, boost::is_any_of(" "));
    if (vs.size() < 2) {
        return false;
    }

    pt_ = std::atoi(vs[0].c_str());
    std::vector<std::string> kvs;
    boost::split(kvs, vs[1], boost::is_any_of(";"));
    for (auto & str : kvs) {
        std::vector<std::string> kv;
        boost::split(kv, str, boost::is_any_of("="));
        if (kv.size() != 2) {
            continue;
        }
        fmt_params_[kv[0]] = kv[1];
    }

    return true;
}

std::string Fmtp::toString() const
{
    std::ostringstream oss;
    std::vector<std::string> params;
    for (auto & kv : fmt_params_) {
        params.push_back(kv.first + "=" + kv.second);
    }

    oss << prefix << pt_ << " " << boost::algorithm::join(params, ";") << std::endl;
    return oss.str();
}