#pragma once

namespace mms {
class RtmpStatsHandler {
public:
    bool processPkt(std::shared_ptr<RtmpMessage> pkt) {
        std::cout <<  "RtmpStatsHandler" << std::endl;
        return true;
    }
};
};