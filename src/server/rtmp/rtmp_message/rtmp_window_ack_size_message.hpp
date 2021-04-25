#pragma once
#include <stdint.h>

namespace mms {
class RtmpWindowAckSizeMessage {
public:
    RtmpWindowAckSizeMessage(uint32_t size) : size_(size) {

    }

    uint32_t size();

    bool encode(char *data, size_t len) {
        return true;
    }
private:
    uint32_t size_;
};
};