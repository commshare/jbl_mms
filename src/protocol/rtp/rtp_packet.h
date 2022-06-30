#pragma once
#include "rtp_header.h"
namespace mms {
    class RtpPacket
    {
    public:
        RtpPacket() {};
        virtual ~RtpPacket() {};

    public:
        int32_t encode(uint8_t *data, size_t len);
        int32_t decode(uint8_t *data, size_t len);
        size_t size();
        uint16_t getSeqNum();
    public:
        RtpHeader header_;
        char *payload_;
        size_t payload_len_;
        // std::shared_ptr<StreamUtil> stream_;
        friend class RtpPacker;
    };
};