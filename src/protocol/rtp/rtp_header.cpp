#include <iostream>
#include <arpa/inet.h>
#include "rtp_header.h"
using namespace mms;
bool RtpHeader::isRtcpPacket(uint8_t *data, size_t len)
{
    if (len < 2)
    {
        return false;
    }
    bool is_rtp_or_rtcp = (len >= 12 && ((data[0] & 0xC0) == 0x80));
    bool is_rtcp_pt     = (data[1] >= 192 && data[1] <= 223);
    
    return is_rtp_or_rtcp && is_rtcp_pt;
}

bool RtpHeader::isRtp(uint8_t *data, size_t len)
{
    bool is_rtp_or_rtcp = (len >= 12 && ((data[0] & 0xC0) == 0x80));
    bool is_rtcp_pt     = (data[1] >= 192 && data[1] <= 223);
    
    return is_rtp_or_rtcp && !is_rtcp_pt;
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
    marker = (tmpv2 >> 7) & 0x01;
    pt = (tmpv2) & 0x7F;
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
    std::cout << "seq=" << seqnum << ", ssrc=" << ssrc << std::endl;
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
        rtp_header_extention = std::make_shared<RtpHeaderExtention>();
        int32_t consumed = rtp_header_extention->decode(data, len);
        if (consumed < 0)
        {
            return -5;
        }
        data += consumed;
        len -= consumed;
    }
    return data - data_start;
}

uint8_t RtpHeader::parsePt(uint8_t *data, size_t len)
{
    return data[1] & 0x7F;
}