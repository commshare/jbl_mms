#pragma once
#include <stdint.h>
#include <vector>
#include <string>

#include "../dtls_define.h"
namespace mms {

#define SRTP_AES128_CM_HMAC_SHA1_80 0x0001
#define SRTP_AEAD_AES_256_GCM       0x0008
#define SRTP_AEAD_AES_128_GCM       0x0007

#define SRTP_AES128_CM_HMAC_SHA1_32 0x0002
#define SRTP_NULL_HMAC_SHA1_80      0x0005
#define SRTP_NULL_HMAC_SHA1_32      0x0006

typedef uint16_t SRTPProtectionProfile;
struct UseSRtpExt : public DtlsExtItem {
    std::vector<SRTPProtectionProfile> profiles;
    std::string srtp_mki;
    int32_t decode(uint8_t *data, size_t len);
    int32_t encode(uint8_t *data, size_t len);
    uint32_t size();
};

};