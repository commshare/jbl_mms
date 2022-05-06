#pragma once
#include <string>
namespace mms {
class Config {
public:
    static Config & getInstance();
    bool loadFromFile(const std::string & config_file);

    uint16_t getWebrtcUdpPort() const {
        return webrtc_udp_port_;
    }
private:
    static Config instance_;

    uint16_t webrtc_udp_port_ = 8878;
};
};