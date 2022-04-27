#include "media_sdp.hpp"
#include <iostream>
#include "base/utils/utils.h"

using namespace mms;
std::string MediaSdp::prefix = "m=";
bool MediaSdp::parse(const std::string & line) {
    std::string::size_type end_pos = line.rfind("\r");
    if (end_pos == std::string::npos) {
        end_pos = line.size() - 1;
    }
    valid_string = line.substr(prefix.size(), end_pos);
    std::vector<std::string> vs;
    vs = Utils::split(valid_string, " ");
    if (vs.size() < 4) {
        return false;
    }

    media = vs[0];
    port = vs[1];
    proto = vs[2];
    for (int i = 3; i < vs.size(); i++) {
        fmts.emplace_back(vs[i]);
    }

    vs = Utils::split(port, "/");
    try {
        if (vs.size() > 1) {
            int iport = std::atoi(vs[0].data());
            int count = std::atoi(vs[1].data());
            for(int i = 0; i < count; i++) {
                ports.push_back(iport + i);
            } 
        } else {
            ports.push_back(std::atoi(vs[0].data()));
        }
    } catch(std::exception & e) {
        return false;
    }

    return true;
}

bool MediaSdp::parseAttr(const std::string & line) {
    if (Utils::startWith(line, IceUfrag::prefix)) {
        IceUfrag ice_ufrag;
        if (!ice_ufrag.parse(line)) {
            return false;
        }
        ice_ufrag_ = ice_ufrag;
        return true;
    } else if (Utils::startWith(line, IcePwd::prefix)) {
        IcePwd ice_pwd;
        if (!ice_pwd.parse(line)) {
            return false;
        }
        ice_pwd_ = ice_pwd;
        return true;
    } else if (Utils::startWith(line, IceOption::prefix)) {
        IceOption ice_option;
        if (!ice_option.parse(line)) {
            return false;
        }
        ice_option_ = ice_option;
        return true;
    } else if (Utils::startWith(line, Extmap::prefix)) {
        Extmap ext_map;
        if (!ext_map.parse(line)) {
            return false;
        }
        ext_maps_.emplace_back(ext_map);
    } else if (Utils::startWith(line, "a=sendonly")) {
        dir_ = sendonly;
    } else if (Utils::startWith(line, "a=sendrecv")) {
        dir_ = sendrecv;
    } else if (Utils::startWith(line, "a=recvonly")) {
        dir_ = recvonly;
    }
    return true;
}