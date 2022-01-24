#pragma once
#include <stdint.h>
#include "server/rtmp/rtmp_protocol/rtmp_define.hpp"

namespace mms {
class RtmpWindowAckSizeMessage : public RtmpMessage {
public:
    RtmpWindowAckSizeMessage(uint32_t size) : RtmpMessage(sizeof(uint32_t)), size_(size) {

    }

    uint32_t size() {
        return 0;
    }

    bool encode(char *data, size_t len) {
        return true;
    }
private:
    uint32_t size_;
};
};