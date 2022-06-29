#include "rtp_packet.h"
using namespace mms;
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
    return data - data_start;
}