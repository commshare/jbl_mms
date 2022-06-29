#pragma once
#include <stdint.h>
#include <memory>
#include <vector>

namespace mms
{

// https://tools.ietf.org/html/rfc3550 @page 11
#define NAL_RTP_PACKET_SIZE 1400

    // RTP Header Extension @page 19
    class RtpHeaderExtention
    {
    public:
        uint16_t profile;
        uint16_t length;
        std::unique_ptr<char[]> header_extention;
        virtual size_t size()
        {
            return 4 + length;
        }
        // todo
        virtual int32_t encode(uint8_t *data, size_t len)
        {
            return 0;
        }

        virtual int32_t decode(uint8_t *data, size_t len)
        {
            return 0;
        }
    };

    class RtpHeader
    {
    public:
        static bool isRtcpPacket(const char *data, size_t len);
        RtpHeader() {};
        // little endian
        uint8_t csrc = 0;      // 4bit
        uint8_t extension = 0; // 1bit
        uint8_t padding = 0;   // 1bit
        uint8_t version = 2;   // 2bit

        uint8_t pt;
        uint8_t marker;

        uint16_t seqnum;
        uint32_t timestamp;
        uint32_t ssrc;
        std::vector<uint32_t> csrcs;
        std::shared_ptr<RtpHeaderExtention> rtp_header_extention = nullptr; // exit if extension bit is set
    public:
        int32_t encode(uint8_t *data, size_t len);
        int32_t decode(uint8_t *data, size_t len);
        size_t size();
    };

};