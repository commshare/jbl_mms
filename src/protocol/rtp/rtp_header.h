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
        // virtual int encode(std::shared_ptr<StreamUtil> stream)
        // {
        //     return 0;
        // }

        // virtual int decode(std::shared_ptr<StreamUtil> stream)
        // {
        //     return 0;
        // }
    };

    class RtpHeader
    {
    public:
        static bool isRtcpPacket(const char *data, size_t len)
        {
            // RTCP版本号必须等于2（kRtcpExpectedVersion）
            if (len < 2)
            {
                return false;
            }

            // const uint8_t V = data[0] >> 6;
            // if (V != 2)
            // {
            //     return false;
            // }

            const uint8_t pt = data[1];
            switch (pt)
            {
            case 192:
                return true;
            case 193:
                // not supported
                // pass through and check for a potential RTP packet
                return false;
            case 195:
            case 200:
            case 201:
            case 202:
            case 203:
            case 204:
            case 205:
            case 206:
            case 207:
                return true;
            default:
                return false;
            }
        }

        RtpHeader();
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
        // int encode(std::shared_ptr<StreamUtil> stream);
        // int decode(std::shared_ptr<StreamUtil> stream);
        size_t size();
    };

    class RtpPacket
    {
    public:
        RtpPacket();
        virtual ~RtpPacket();

    public:
        // int encode(std::shared_ptr<StreamUtil> stream);
        // int decode(std::shared_ptr<StreamUtil> stream);
        size_t size();
        uint16_t getSeqNum();

    private:
        RtpHeader header_;
        char *payload_;
        size_t payload_len_;
        // std::shared_ptr<StreamUtil> stream_;
        friend class RtpPacker;
    };

};