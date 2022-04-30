#include <boost/algorithm/string.hpp>
#include "sdp.hpp"
#include "base/utils/utils.h"
#include <iostream>
using namespace mms;
int32_t Sdp::parse(const std::string & sdp) {
    std::cout << "sdp:" << sdp << std::endl;
    std::vector<std::string> lines = Utils::split(sdp, "\n");
    if (lines.size() <= 3) {
        return -1;
    }

    // protocol version
    if (!boost::starts_with(lines[0], ProtocolVersion::prefix)) {
        return -1;
    }

    if (!protocol_version_.parse(lines[0])) {
        return -2;
    }
    // origin
    if (!boost::starts_with(lines[1], Origin::prefix)) {
        return -3;
    }
    if (!origin_.parse(lines[1])) {
        return -4;
    }
    // session_name
    if (!boost::starts_with(lines[2], SessionName::prefix)) {
        return -5;
    }
    if (!session_name_.parse(lines[2])) {
        return -6;
    }

    std::optional<MediaSdp> curr_media_sdp;
    for (size_t i = 2; i < lines.size(); i++) {
        std::cout << "parse line:" << lines[i] << std::endl;
        if (boost::starts_with(lines[i], MediaSdp::prefix)) {
            if (curr_media_sdp.has_value()) {
                std::cout << "add media:" << curr_media_sdp.value().getMedia() << std::endl;
                media_sdps_.emplace_back(curr_media_sdp.value());
            } 

            MediaSdp msdp;
            if (!msdp.parse(lines[i])) {
                std::cout << "parse media sdp failed." << std::endl;
                return -7;
            }
            curr_media_sdp = msdp;
            continue;
        }

        if (curr_media_sdp.has_value()) {
            if (!(*curr_media_sdp).parseAttr(lines[i])) {
                return -8;
            }
        } else {
            // session information
            if (boost::starts_with(lines[i], SessionInformation::prefix)) {
                SessionInformation si;
                if (!si.parse(lines[i])) {
                    return -9;
                }
                session_info_ = si;
            } else if (boost::starts_with(lines[i], Uri::prefix)) {
                Uri uri;
                if (!uri.parse(lines[i])) {
                    return -10;
                }
                uri_ = uri;
            } else if (boost::starts_with(lines[i], EmailAddress::prefix)) {
                EmailAddress email;
                if (!email.parse(lines[i])) {
                    return -11;
                }
                email_ = email;
            } else if (boost::starts_with(lines[i], Phone::prefix)) {
                Phone phone;
                if (!phone.parse(lines[i])) {
                    return -12;
                }
                phone_ = phone;
            } else if (boost::starts_with(lines[i], ConnectionInfo::prefix)) {
                ConnectionInfo conn_info;
                if (!conn_info.parse(lines[i])) {
                    return -13;
                }
                conn_info_ = conn_info;
            } else if (boost::starts_with(lines[i], BundleAttr::prefix)) {
                BundleAttr bundle_attr;
                if (!bundle_attr.parse(lines[i])) {
                    return -14;
                }
                bundle_attr_ = bundle_attr;
            }
        }
    }

    if (curr_media_sdp.has_value()) {
        std::cout << "add media:" << curr_media_sdp.value().getMedia() << std::endl;
        media_sdps_.emplace_back(curr_media_sdp.value());
    }
    
    return 0;
}

int Sdp::getVersion() {

}

void Sdp::setVersion(int v) {
    
}