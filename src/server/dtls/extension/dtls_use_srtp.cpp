#include <arpa/inet.h>
#include <stdint.h>

#include "dtls_use_srtp.h"
using namespace mms;

int32_t UseSRtpExt::decode(uint8_t *data, size_t len)
{
    uint8_t *data_start = data;
    int32_t c = header.decode(data, len);
    if (c < 0) {
        return -1;
    }
    data += c;
    len -= c;

    if (len < 2) {
        return -2;
    }

    uint16_t length = ntohs(*(uint16_t*)data);
    data += 2;
    len -= 2;
    while (length > 0) {
        if (len < 2) {
            return -3;
        }

        SRTPProtectionProfile profile = (SRTPProtectionProfile)ntohs(*(uint16_t *)data);
        data += 2;
        length -= 2;
        len -= 2;
        profiles.push_back(profile);
    }

    return data - data_start;
}