#include "rtp_packet.h"
using namespace mms;

RtpPacket::~RtpPacket()
{
    if (payload_)
    {
        delete payload_;
        payload_len_ = 0;
    }
}
int32_t RtpPacket::decode(uint8_t *data, size_t len)
{
    uint8_t *data_start = data;
    int32_t consumed = header_.decode(data, len);
    if (consumed < 0)
    {
        return -1;
    }
    data += consumed;
    len -= consumed;

    payload_ = new char[len];
    memcpy(payload_, data, len);
    payload_len_ = len;
    data += len;
    len = 0;
    return data - data_start;
}

std::pair<char*, size_t> RtpPacket::getPayload()
{

}