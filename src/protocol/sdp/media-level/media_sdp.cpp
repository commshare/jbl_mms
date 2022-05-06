#include <sstream>
#include <boost/algorithm/string.hpp>
#include "media_sdp.hpp"
#include "rtcp_fb.h"
#include <iostream>
#include "base/utils/utils.h"

using namespace mms;
std::string MediaSdp::prefix = "m=";
bool MediaSdp::parse(const std::string &line)
{
    std::string::size_type end_pos = line.rfind("\r");
    if (end_pos == std::string::npos)
    {
        end_pos = line.size() - 1;
    }
    std::string valid_string = line.substr(prefix.size(), end_pos - prefix.size());
    std::vector<std::string> vs;
    boost::split(vs, valid_string, boost::is_any_of(" "));
    if (vs.size() < 3)
    {
        return false;
    }

    media = vs[0];
    std::string &sport = vs[1];
    proto = vs[2];
    for (size_t i = 3; i < vs.size(); i++)
    {
        fmts.emplace_back(std::atoi(vs[i].c_str()));
    }

    vs = Utils::split(sport, "/");
    try
    {
        if (vs.size() > 1)
        {
            port = std::atoi(vs[0].c_str());
            port_count = std::atoi(vs[1].c_str());
        }
        else
        {
            port = std::atoi(vs[0].c_str());
        }
    }
    catch (std::exception &e)
    {
        return false;
    }

    return true;
}

bool MediaSdp::parseAttr(const std::string &line)
{
    if (boost::starts_with(line, IceUfrag::prefix))
    {
        IceUfrag ice_ufrag;
        if (!ice_ufrag.parse(line))
        {
            return false;
        }
        ice_ufrag = ice_ufrag;
        return true;
    }
    else if (boost::starts_with(line, IcePwd::prefix))
    {
        IcePwd ice_pwd;
        if (!ice_pwd.parse(line))
        {
            return false;
        }
        ice_pwd = ice_pwd;
        return true;
    }
    else if (boost::starts_with(line, IceOption::prefix))
    {
        IceOption ice_option;
        if (!ice_option.parse(line))
        {
            return false;
        }
        ice_option = ice_option;
        return true;
    }
    else if (boost::starts_with(line, Extmap::prefix))
    {
        Extmap ext_map;
        if (!ext_map.parse(line))
        {
            return false;
        }
        ext_maps.emplace_back(ext_map);
    }
    else if (DirAttr::isMyPrefix(line))
    {
        if (!dir.parse(line))
        {
            return false;
        }
    }
    else if (boost::starts_with(line, ConnectionInfo::prefix))
    {
        ConnectionInfo ci;
        if (!ci.parse(line))
        {
            return false;
        }
        connection_info = ci;
    }
    else if (boost::starts_with(line, MidAttr::prefix))
    {
        MidAttr ma;
        if (!ma.parse(line))
        {
            return false;
        }
        mid = ma;
    }
    else if (Payload::isMyPrefix(line))
    {
        Payload payload;
        if (!payload.parse(line))
        {
            return false;
        }
        curr_pt = payload.getPt();
        payloads_.insert(std::pair(payload.getPt(), payload));
    }
    else if (RtcpFb::isMyPrefix(line))
    {
        if (0 != payloads_[curr_pt].parseRtcpFbAttr(line))
        {
            return false;
        }
    }
    else if (Fmtp::isMyPrefix(line))
    {
        if (0 != payloads_[curr_pt].parseFmtpAttr(line))
        {
            return false;
        }
    }
    else if (boost::starts_with(line, MaxPTimeAttr::prefix))
    {
        MaxPTimeAttr attr;
        if (!attr.parse(line))
        {
            return false;
        }
        max_ptime = attr;
    }
    else if (boost::starts_with(line, Ssrc::prefix))
    {
        if (!ssrc_.parse(line))
        {
            return false;
        }
    }
    else if (boost::starts_with(line, SetupAttr::prefix))
    {
        if (!setup_.parse(line))
        {
            return false;
        }
    }
    return true;
}

std::string MediaSdp::toString() const
{
    std::ostringstream oss;
    {
        oss << prefix << media << " " << proto;
        for (size_t i = 0; i < fmts.size(); i++)
        {
            if (i != fmts.size() - 1)
            {
                oss << " " << std::to_string(fmts[i]) << " ";
            }
            else
            {
                oss << " " << std::to_string(fmts[i]);
            }
        }
        oss << std::endl;
    }

    if (connection_info)
    {
        oss << connection_info.value().toString();
    }

    if (ice_ufrag)
    {
        oss << ice_ufrag.value().toString();
    }

    if (ice_pwd)
    {
        oss << ice_pwd.value().toString();
    }

    if (ice_option)
    {
        oss << ice_option.value().toString();
    }

    for (auto &c : candidates_)
    {
        oss << c.toString();
    }

    if (rtcp_mux_)
    {
        oss << rtcp_mux_.value().toString();
    }

    for (auto &p : payloads_)
    {
        oss << p.second.toString();
    }

    oss << dir.toString();
    oss << setup_.toString();
    oss << mid.toString();
    oss << ssrc_.toString();

    return oss.str();
}