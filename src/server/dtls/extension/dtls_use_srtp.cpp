#include <iostream>

#include <string.h>
#include <arpa/inet.h>
#include <stdint.h>

#include "dtls_use_srtp.h"
using namespace mms;

int32_t UseSRtpExt::decode(uint8_t *data, size_t len)
{
    uint8_t *data_start = data;
    int32_t c = header.decode(data, len);
    if (c < 0)
    {
        return -1;
    }
    data += c;
    len -= c;

    if (len < 2)
    {
        return -2;
    }

    uint16_t length = ntohs(*(uint16_t *)data);
    uint16_t count = length > 1;
    data += 2;
    len -= 2;
    while (count > 0)
    {
        if (len < 2)
        {
            return -3;
        }

        SRTPProtectionProfile profile = (SRTPProtectionProfile)ntohs(*(uint16_t *)data);
        data += 2;
        count--;
        len -= 2;
        profiles.push_back(profile);
    }

    int16_t left_length = header.length - (length + 2);
    if (left_length > 0)
    {
        srtp_mki.assign((char *)data, left_length);
        data += left_length;
    }

    return data - data_start;
}

uint32_t UseSRtpExt::size()
{
    uint32_t size = 0;
    size += header.size();
    size += 2; // profiles length
    size += profiles.size() * 2;
    size += srtp_mki.size() + 1;
    return size;
}

int32_t UseSRtpExt::encode(uint8_t *data, size_t len)
{
    uint8_t *data_start = data;
    int32_t c = header.encode(data, len);
    if (c < 0)
    {
        return -1;
    }
    data += c;
    len -= c;

    uint16_t *plen = (uint16_t *)data;
    data += 2;
    len -= 2;
    if (len < 0)
    {
        return -2;
    }

    uint32_t content_len = 0;
    for (auto &profile : profiles)
    {
        *(uint16_t *)data = htons(profile);
        data += 2;
        len -= 2;
        content_len += 2;
    }

    memcpy(data, srtp_mki.c_str(), srtp_mki.size());
    data += srtp_mki.size();
    len -= srtp_mki.size();
    content_len += srtp_mki.size();

    data[0] = 0;
    data++;
    len--;
    content_len++;

    *plen = htons(content_len);
    return data - data_start;
}
