#include "rtp_h264_packet.h"
using namespace mms;

H264RtpPacket::H264RtpPacket()
{

}

H264RtpPacket::~H264RtpPacket()
{

}

int32_t H264RtpPacket::decode(uint8_t *data, size_t len)
{
    int32_t consumed = RtpPacket::decode(data, len);
    if (payload_len_ < 1)
    {
        return -1;
    }

    type = (H264_RTP_HEADER_TYPE)(payload_[0] & 0x1F);
    return consumed;
}

int32_t H264RtpPacket::encode(uint8_t *data, size_t len)
{
    return 0;
}


size_t H264RtpPacket::size()
{
    return 0;
}