#pragma once

namespace mms {
class RtmpBandwidthHandler {
public:
    bool processPkt(std::shared_ptr<RtmpMessage> pkt) {
        std::cout <<  "RtmpBandwidthHandler" << std::endl;
        return true;
    }
};

};