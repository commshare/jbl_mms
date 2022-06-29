#include <arpa/inet.h>
#include "rtp_header.h"
using namespace mms;
bool RtpHeader::isRtcpPacket(const char *data, size_t len)
{
    if (len < 2)
    {
        return false;
    }

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

int32_t RtpHeader::decode(uint8_t *data, size_t len)
{
    uint8_t *data_start = data;
    if (len < 1)
    {
        return -1;
    }

    uint8_t tmpv1 = *data;
    csrc = (tmpv1 >> 0) & 0x0F;
    extension = (tmpv1 >> 4) & 0x01;
    padding = (tmpv1 >> 5) & 0x01;
    version = (tmpv1 >> 6) & 0x03;

    data++;
    len--;
    if (len < 1)
    {
        return -2;
    }

    uint8_t tmpv2 = *data;
    marker = (tmpv2 >> 0) & 0x01;
    pt = (tmpv2 >> 1) & 0x7F;
    data++;
    len--;
    //seqnum
    if (len < 2)
    {
        return -3;
    }

    seqnum = ntohs(*(uint16_t*)data);
    data += 2;
    len -= 2;
    //timestamp
    if (len < 4)
    {
        return -4;
    }

    timestamp = ntohl(*(uint32_t*)data);
    data += 4;
    len -= 4;
    //ssrc
    if (len < 4)
    {
        return -4;
    }

    ssrc = ntohl(*(uint32_t*)data);
    data += 4;
    len -= 4;

    for(int i = 0; i < csrc; i++)
    {
        if (len < 4)
        {
            return -5;
        }
        auto c = ntohl(*(uint32_t*)data);
        data += 4;
        len -= 4;
        csrcs.push_back(c);
    }

    if(extension)
    {
        // rtp_header_extention = std::make_shared<RtpHeaderExtention>();
        // if(0 != rtp_header_extention->decode(stream))
        // {
        //     return -5;
        // }
    }
    return data - data_start;
}