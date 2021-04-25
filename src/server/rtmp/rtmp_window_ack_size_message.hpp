#pragma once
#include <stdint.h>
#include "rtmp_protocol.hpp"
namespace mms {
class RtmpWindowAckSizeMessage : public RtmpMessage {
public:
    RtmpWindowAckSizeMessage(uint32_t size) : size_(size), RtmpMessage(sizeof(uint32_t)) {

    }

    uint32_t size();

    bool encode(char *data, size_t len) {
        return true;
    }
private:
    uint32_t size_;
};
};