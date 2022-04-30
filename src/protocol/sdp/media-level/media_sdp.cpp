#include "media_sdp.hpp"
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
    std::string valid_string = line.substr(prefix.size(), end_pos);
    std::vector<std::string> vs;
    vs = Utils::split(valid_string, " ");
    if (vs.size() < 3)
    {
        return false;
    }

    media = vs[0];
    std::string &port = vs[1];
    proto = vs[2];
    for (size_t i = 3; i < vs.size(); i++)
    {
        fmts.emplace_back(std::atoi(vs[i].c_str()));
    }

    vs = Utils::split(port, "/");
    try
    {
        if (vs.size() > 1)
        {
            int iport = std::atoi(vs[0].c_str());
            int count = std::atoi(vs[1].c_str());
            for (int i = 0; i < count; i++)
            {
                ports.push_back(iport + i);
            }
        }
        else
        {
            ports.push_back(std::atoi(vs[0].c_str()));
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
    if (Utils::startWith(line, IceUfrag::prefix))
    {
        IceUfrag ice_ufrag;
        if (!ice_ufrag.parse(line))
        {
            return false;
        }
        ice_ufrag = ice_ufrag;
        return true;
    }
    else if (Utils::startWith(line, IcePwd::prefix))
    {
        IcePwd ice_pwd;
        if (!ice_pwd.parse(line))
        {
            return false;
        }
        ice_pwd = ice_pwd;
        return true;
    }
    else if (Utils::startWith(line, IceOption::prefix))
    {
        IceOption ice_option;
        if (!ice_option.parse(line))
        {
            return false;
        }
        ice_option = ice_option;
        return true;
    }
    else if (Utils::startWith(line, Extmap::prefix))
    {
        Extmap ext_map;
        if (!ext_map.parse(line))
        {
            return false;
        }
        ext_maps.emplace_back(ext_map);
    }
    else if (Utils::startWith(line, "a=sendonly"))
    {
        dir = sendonly;
    }
    else if (Utils::startWith(line, "a=sendrecv"))
    {
        dir = sendrecv;
    }
    else if (Utils::startWith(line, "a=recvonly"))
    {
        dir = recvonly;
    }
    else if (Utils::startWith(line, ConnectionInfo::prefix))
    {
        ConnectionInfo ci;
        if (!ci.parse(line))
        {
            return false;
        }
        connection_info = ci;
    }
    else if (Utils::startWith(line, MidAttr::prefix))
    {
        MidAttr ma;
        if (!ma.parse(line))
        {
            return false;
        }
        mid = ma;
    }
    return true;
}