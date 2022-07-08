#include <iostream>
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
    // std::cout << "header consumed:" << consumed << std::endl;
    // printf("rtp:");
    // for (int i = 0; i < 30; i++) {
    //     printf("%02x ", data[i]);
    // }
    // printf("\r\n");
    data += consumed;
    len -= consumed;

    payload_ = new char[len];
    memcpy(payload_, data, len);
    payload_len_ = len;
    data += len;
    len = 0;
    return data - data_start;
}

int32_t RtpPacket::encode(uint8_t *data, size_t len)
{
    return 0;
}

size_t RtpPacket::size()
{
    return 0;
}

std::pair<char*, size_t> RtpPacket::getPayload()
{
    return std::pair(payload_, payload_len_);
}

uint16_t RtpPacket::getSeqNum()
{
    return header_.seqnum;
}

uint32_t RtpPacket::getTimestamp()
{
    return header_.timestamp;
}