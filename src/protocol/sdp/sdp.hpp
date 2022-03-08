#pragma once
#include <set>
#include <string>

#include "session-level/protocol_version.hpp"
namespace mms {
struct Sdp {
public:
    int32_t parse(const std::string & sdp);
private:
    std::string raw_str;
    std::set<std::string> candidates;
private:
    ProtocolVersion protocol_version_;
};
};