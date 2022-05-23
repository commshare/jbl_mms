#include <arpa/inet.h>
#include <stdint.h>

#include "dtls_use_srtp.h"
using namespace mms;

int32_t UseSRtpExt::decode(uint8_t *data, size_t len)
{
    uint8_t *data_start = data;
    if (len < 2) {
        return -1;
    }

    uint16_t length = ntohs(*(uint16_t*)data);
    int16_t count = length >> 1;
    while (count > 0) {
        if (len < 2) {
            return -2;
        }

        SRTPProtectionProfile profile = (SRTPProtectionProfile)ntohs(*(uint16_t *)data);
        data += 2;
        count--;
        len -= 2;
        profiles.push_back(profile);
    }

    return data - data_start;
}