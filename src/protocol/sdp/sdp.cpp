#include "sdp.hpp"
#include "base/utils/utils.h"
#include <iostream>
using namespace mms;
int32_t Sdp::parseRemoteSdp(const std::string & sdp) {
    raw_str = std::move(sdp);
    std::cout << "sdp:" << raw_str << std::endl;
    lines_ = Utils::split(raw_str, "\n");
    if (lines_.size() <= 3) {
        return -1;
    }

    // protocol version
    if (!Utils::startWith(lines_[0], ProtocolVersion::prefix)) {
        return -1;
    }

    if (!protocol_version_.parse(lines_[0])) {
        return -2;
    }
    // origin
    if (!Utils::startWith(lines_[1], Origin::prefix)) {
        return -3;
    }
    if (!origin_.parse(lines_[1])) {
        return -4;
    }
    // session_name
    if (!Utils::startWith(lines_[2], SessionName::prefix)) {
        return -5;
    }
    if (session_name_.parse(lines_[2])) {
        return -6;
    }

    std::optional<MediaSdp> currMediaSdp;
    for (size_t i = 2; i < lines_.size(); i++) {
        if (Utils::startWith(lines_[i], MediaSdp::prefix)) {
            if (currMediaSdp) {
                media_sdps_.emplace_back(currMediaSdp.value());
            } else {
                MediaSdp msdp;
                if (!msdp.parse(lines_[i])) {
                    return false;
                }
                currMediaSdp = msdp;
            }
            continue;
        }

        if (currMediaSdp.has_value()) {
            if (!(*currMediaSdp).parseAttr(lines_[i])) {
                return false;
            }
        } else {
            // session information
            if (Utils::startWith(lines_[i], SessionInformation::prefix)) {
                SessionInformation si;
                if (!si.parse(lines_[i])) {
                    return -7;
                }
                session_info_ = si;
                continue;
            } else if (Utils::startWith(lines_[i], Uri::prefix)) {
                Uri uri;
                if (!uri.parse(lines_[i])) {
                    return -7;
                }
                uri_ = uri;
            } else if (Utils::startWith(lines_[i], EmailAddress::prefix)) {
                EmailAddress email;
                if (!email.parse(lines_[i])) {
                    return -7;
                }
                email_ = email;
            } else if (Utils::startWith(lines_[i], Phone::prefix)) {
                Phone phone;
                if (!phone.parse(lines_[i])) {
                    return -7;
                }
                phone_ = phone;
            } else if (Utils::startWith(lines_[i], ConnectionInfo::prefix)) {
                ConnectionInfo conn_info;
                if (!conn_info.parse(lines_[i])) {
                    return -7;
                }
                conn_info_ = conn_info;
            } else if (Utils::startWith(lines_[i], BundleAttr::prefix)) {
                BundleAttr bundle_attr;
                if (!bundle_attr.parse(lines_[i])) {
                    return -7;
                }
                bundle_attr_ = bundle_attr;
            }
        }
    }

    if (currMediaSdp.has_value()) {
        media_sdps_.emplace_back(currMediaSdp.value());
    }
    
    return 0;
}

int32_t Sdp::createLocalSdp(Sdp & local_sdp) {
    return 0;
}