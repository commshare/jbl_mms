#pragma once
#include "rtp_header.h"
namespace mms {
    class RtpPacket
    {
    public:
        RtpPacket() {};
        virtual ~RtpPacket();

    public:
        virtual int32_t encode(uint8_t *data, size_t len);
        virtual int32_t decode(uint8_t *data, size_t len);
        virtual size_t size();
        uint16_t getSeqNum();

        std::pair<char*, size_t> getPayload();
    public:
        RtpHeader header_;
        char *payload_ = nullptr;
        size_t payload_len_ = 0;
    };
};