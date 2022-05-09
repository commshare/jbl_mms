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
#include "session-level/timing.hpp"
#include "session-level/tool.hpp"
#include "attribute/common/dir.hpp"
#include "session-level/ssrc_group.h"
#include "protocol/sdp/ice/ice_ufrag.h"
#include "protocol/sdp/ice/ice_pwd.h"

#include "media-level/media_sdp.hpp"
#include "webrtc/ssrc.h"

namespace mms {
struct Sdp {
public:
    int32_t parse(const std::string & sdp);
    int getVersion();
    void setVersion(int v);

    void setOrigin(const Origin &origin) {
        origin_ = origin;
    }

    Origin & getOrigin() {
        return origin_;
    }

    void setSessionName(const SessionName & session_name) {
        session_name_ = session_name;
    }

    SessionName & getSessionName() {
        return session_name_;
    }

    void setTime(const Timing & time) {
        time_ = time;
    }

    const Timing & getTime() {
        return time_;
    }

    void setTool(const ToolAttr & tool) {
        tool_ = tool;
    }

    const ToolAttr & getTool() {
        return tool_.value();
    }

    void setBundle(const BundleAttr & bundle) {
        bundle_attr_ = bundle;
    }

    const BundleAttr & getBundle() {
        return bundle_attr_.value();
    }

    const std::vector<MediaSdp> & getMediaSdps() const {
        return media_sdps_;
    }

    void setMediaSdps(const std::vector<MediaSdp> & media_sdps) {
        media_sdps_ = media_sdps;
    }

    void addMediaSdp(const MediaSdp & media_sdp) {
        media_sdps_.emplace_back(media_sdp);
    }

    void addAttr(const std::string & val) {
        attrs_.emplace_back(val);
    }
    
    const std::optional<IceUfrag> & getIceUfrag() const {
        if (ice_ufrag_) {
            return ice_ufrag_;
        }

        for (auto & media : media_sdps_) {
            return media.getIceUfrag();
        }
        return std::nullopt;
    }

    const std::optional<IcePwd> & getIcePwd() const {
        if (ice_pwd_) {
            return ice_pwd_;
        }

        for (auto & media : media_sdps_) {
            return media.getIcePwd();
        }
        return std::nullopt;
    }

    std::string toString() const;
protected:
    ProtocolVersion protocol_version_;
    Origin          origin_;
    SessionName     session_name_;
    Timing          time_;
    std::vector<std::string> attrs_;
    
    std::optional<SessionInformation> session_info_;
    std::optional<ToolAttr> tool_;
    std::optional<DirAttr> dir_;
    std::optional<Uri> uri_;
    std::optional<EmailAddress> email_;
    std::optional<Phone> phone_;
    std::optional<ConnectionInfo> conn_info_;
    std::optional<IceUfrag> ice_ufrag_;
    std::optional<IcePwd> ice_pwd_;
    std::optional<BundleAttr> bundle_attr_;
    
    std::vector<MediaSdp> media_sdps_;
    std::optional<SsrcGroup> ssrc_group_;
};
};