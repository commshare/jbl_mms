#include <iostream>
#include <sstream>
#include <boost/algorithm/string.hpp>
#include "sdp.hpp"
#include "base/utils/utils.h"

using namespace mms;
int32_t Sdp::parse(const std::string &sdp)
{
    std::cout << "sdp:" << sdp << std::endl;
    std::vector<std::string> lines = Utils::split(sdp, "\n");
    if (lines.size() <= 3)
    {
        return -1;
    }
    std::cout << "line count:" << lines.size() << std::endl;
    // protocol version
    if (!boost::starts_with(lines[0], ProtocolVersion::prefix))
    {
        return -1;
    }

    if (!protocol_version_.parse(lines[0]))
    {
        return -2;
    }
    // origin
    if (!boost::starts_with(lines[1], Origin::prefix))
    {
        return -3;
    }
    if (!origin_.parse(lines[1]))
    {
        return -4;
    }
    // session_name
    if (!boost::starts_with(lines[2], SessionName::prefix))
    {
        return -5;
    }

    if (!session_name_.parse(lines[2]))
    {
        return -6;
    }

    size_t i = 3;
    while (!boost::starts_with(lines[i], MediaSdp::prefix))
    { //在m=之前，都是session的属性
        if (DirAttr::isMyPrefix(lines[i]))
        {
            DirAttr dir;
            if (!dir.parse(lines[i]))
            {
                return -7;
            }
            dir_ = dir;
        }
        else if (boost::starts_with(lines[i], Uri::prefix))
        {
            Uri uri;
            if (!uri.parse(lines[i]))
            {
                return -10;
            }
            uri_ = uri;
        }
        else if (boost::starts_with(lines[i], EmailAddress::prefix))
        {
            EmailAddress email;
            if (!email.parse(lines[i]))
            {
                return -11;
            }
            email_ = email;
        }
        else if (boost::starts_with(lines[i], Phone::prefix))
        {
            Phone phone;
            if (!phone.parse(lines[i]))
            {
                return -12;
            }
            phone_ = phone;
        }
        else if (boost::starts_with(lines[i], ConnectionInfo::prefix))
        {
            ConnectionInfo conn_info;
            if (!conn_info.parse(lines[i]))
            {
                return -13;
            }
            conn_info_ = conn_info;
        }
        else if (boost::starts_with(lines[i], SessionInformation::prefix))
        {
            SessionInformation si;
            if (!si.parse(lines[i]))
            {
                return -9;
            }
            session_info_ = si;
        }
        else if (boost::starts_with(lines[i], Timing::prefix))
        {
            if (!time_.parse(lines[i]))
            {
                return -10;
            }
        }
        else if (boost::starts_with(lines[i], ConnectionInfo::prefix))
        {
            ConnectionInfo conn_info;
            if (!conn_info.parse(lines[i]))
            {
                return -13;
            }
            conn_info_ = conn_info;
        }

        i++;
    }

    std::optional<MediaSdp> curr_media_sdp;
    for (; i < lines.size(); i++)
    {
        std::cout << "parse line:" << lines[i] << std::endl;
        if (boost::starts_with(lines[i], MediaSdp::prefix))
        {
            if (curr_media_sdp.has_value())
            {
                std::cout << "add media:" << curr_media_sdp.value().getMedia() << std::endl;
                media_sdps_.emplace_back(curr_media_sdp.value());
            }

            MediaSdp msdp;
            if (!msdp.parse(lines[i]))
            {
                std::cout << "parse media sdp failed." << std::endl;
                return -7;
            }
            curr_media_sdp = msdp;
            continue;
        } else if (boost::starts_with(lines[i], SsrcGroup::prefix)) {
            break;
        }

        bool ret = false;
        if (curr_media_sdp.has_value())
        {
            ret = (*curr_media_sdp).parseAttr(lines[i]);
        }

        if (!ret)
        {
            if (boost::starts_with(lines[i], BundleAttr::prefix))
            {
                BundleAttr bundle_attr;
                if (!bundle_attr.parse(lines[i]))
                {
                    return -14;
                }
                bundle_attr_ = bundle_attr;
            }
        }
    }

    if (curr_media_sdp.has_value())
    {
        std::cout << "add media:" << curr_media_sdp.value().getMedia() << std::endl;
        media_sdps_.emplace_back(curr_media_sdp.value());
    }

    for (; i < lines.size(); i++) {
        if (boost::starts_with(lines[i], SsrcGroup::prefix)) {
            SsrcGroup ssrc_group;
            if (!ssrc_group.parse(lines[i])) {
                return -8;
            }
            ssrc_group_ = ssrc_group;
            continue;
        }

        if (ssrc_group_) {
            if (!ssrc_group_.value().parseSsrc(lines[i])) {
                return -9;
            }
        }
    }
    

    return 0;
}

int Sdp::getVersion()
{
    return protocol_version_.getVersion();
}

void Sdp::setVersion(int v)
{
    protocol_version_.setVersion(v);
}

std::string Sdp::toString() const
{
    std::ostringstream oss;
    oss << protocol_version_.toString();
    oss << origin_.toString();
    oss << session_name_.toString();
    oss << time_.toString();
    if (session_info_) {
        oss << session_info_.value().toString();
    }

    if (tool_) {
        oss << tool_.value().toString();
    }

    for (auto & a : attrs_) {
        oss << "a=" << a << std::endl; 
    }

    if (bundle_attr_) {
        oss << bundle_attr_.value().toString();
    }

    for (auto & m : media_sdps_) {
        oss << m.toString();
    }
    // std::optional<ToolAttr> tool_;
    // std::optional<DirAttr> dir_;
    // std::optional<Uri> uri_;
    // std::optional<EmailAddress> email_;
    // std::optional<Phone> phone_;
    // std::optional<ConnectionInfo> conn_info_;
    // std::optional<BundleAttr> bundle_attr_;
    
    // std::vector<MediaSdp> media_sdps_;
    // std::set<std::string> candidates_;
    // std::unordered_map<uint32_t, Ssrc> ssrcs_;

    return oss.str();
}