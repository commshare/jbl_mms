#pragma once
#include <set>
#include <string>
#include <optional>
#include <vector>
#include <unordered_map>

#include "session-level/protocol_version.hpp"
#include "session-level/origin.hpp"
#include "session-level/session_name.hpp"
#include "session-level/session_information.hpp"
#include "session-level/uri.hpp"
#include "session-level/email.hpp"
#include "session-level/phone.hpp"
#include "session-level/connection_info.hpp"
#include "session-level/bundle.hpp"

#include "media-level/media_sdp.hpp"

namespace mms {
struct Sdp {
public:
    int32_t parse(const std::string & sdp);
    int getVersion();
    void setVersion(int v);
private:
    ProtocolVersion protocol_version_;
    Origin          origin_;
    SessionName     session_name_;
    std::optional<SessionInformation> session_info_;
    std::optional<Uri> uri_;
    std::optional<EmailAddress> email_;
    std::optional<Phone> phone_;
    std::optional<ConnectionInfo> conn_info_;
    std::optional<BundleAttr> bundle_attr_;
    
    std::vector<MediaSdp> media_sdps_;
    std::set<std::string> candidates;
};
};