#pragma once
#include <memory>

namespace mms {
class RtmpCodecParser {
public:
    RtmpCodecParser() {

    }

    virtual ~RtmpCodecParser() {

    }

    bool processPkt(std::shared_ptr<RtmpMessage> pkt) {
        return true;
    }
};

};