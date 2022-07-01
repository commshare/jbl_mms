#pragma once
#include "rtp_packet.h"
namespace mms {
enum H264_RTP_HEADER_TYPE {
    H264_RTP_PAYLOAD_UNKNOWN             = 0,
    H264_RTP_PAYLOAD_SINGLE_NALU_START   = 1,
    H264_RTP_PAYLOAD_SINGLE_NALU_END     = 23,
    H264_RTP_PAYLOAD_STAP_A              = 24,
    H264_RTP_PAYLOAD_STAP_B              = 25,
    H264_RTP_PAYLOAD_MTAP16              = 26,
    H264_RTP_PAYLOAD_MTAP32              = 27,
    H264_RTP_PAYLOAD_FU_A                = 28,
    H264_RTP_PAYLOAD_FU_B                = 29,
};

class H264RtpPacket : public RtpPacket {
public:
    H264RtpPacket();
    virtual ~H264RtpPacket();
public:
    int32_t decode(uint8_t *data, size_t len);
    int32_t encode(uint8_t *data, size_t len);
    size_t size();
    H264_RTP_HEADER_TYPE getType() const {
        return type;
    }

    uint8_t getNaluType() const {
        return nalu_type;
    }

    uint8_t isStartFU() const {
        return start_bit;
    }

    uint8_t isEndFU() const {
        return end_bit;
    }
private:
    uint8_t marker;
    uint8_t nalu_type;
    H264_RTP_HEADER_TYPE type;
    // when fu-a
    uint8_t start_bit;
    uint8_t end_bit;
};

};