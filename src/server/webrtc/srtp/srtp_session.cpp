#include <string.h>
#include "srtp_session.h"
using namespace mms;

bool SRTPSession::init(SRTPProtectionProfile profile, const std::string &recv_key, const std::string &send_key)
{
    srtp_policy_t srtp_policy;
    memset(&srtp_policy, 0, sizeof(srtp_policy_t));
    switch (profile)
    {
    case SRTP_AES128_CM_HMAC_SHA1_80:
        srtp_crypto_policy_set_aes_cm_128_hmac_sha1_80(&srtp_policy.rtp);
        srtp_crypto_policy_set_aes_cm_128_hmac_sha1_80(&srtp_policy.rtcp);
        break;
    default:
        break;
    }

    if (recv_key.size() != srtp_policy.rtp.cipher_key_len) {
        std::cout << "recv_key was not match, len:" << recv_key.size() 
                << ", expected:" << srtp_policy.rtp.cipher_key_len << std::endl;
    }
    
    if (send_key.size() != srtp_policy.rtp.cipher_key_len) {
        std::cout << "send_key was not match, len:" << send_key.size() 
                << ", expected:" << srtp_policy.rtp.cipher_key_len << std::endl;
    };


    srtp_policy.ssrc.value = 0;
    srtp_policy.allow_repeat_tx = 1;
    // srtp_policy.window_size = 1024;
    srtp_policy.window_size = 4096;
    srtp_policy.next = NULL;

    srtp_policy.ssrc.type = ssrc_any_inbound;
    srtp_policy.key = (unsigned char *)(recv_key.c_str());
    auto err = srtp_create(&recv_ctx_, &srtp_policy);
    if (err != srtp_err_status_ok)
    {
        return false;
    }

    srtp_policy.ssrc.type = ssrc_any_outbound;
    srtp_policy.key = (unsigned char *)(send_key.c_str());
    err = srtp_create(&send_ctx_, &srtp_policy);
    if (err != srtp_err_status_ok)
    {
        return false;
    }

    return true;
}

int32_t SRTPSession::unprotectSRTP(uint8_t *data, size_t len)
{
    if (len >= SRTP_MAX_BUFFER_SIZE)
    {
        return -1;
    }
    std::cout << "unprotect srtp len:" << len << std::endl;
    int out_len = len;
    auto err = srtp_unprotect(recv_ctx_, data, (int*)&out_len);
    if (err != srtp_err_status_ok)
    {
        std::cout << "decode srtp failed, code:" << err << std::endl;
        return -2;
    }
    std::cout << "unprotect srtp out len:" << out_len << std::endl;
    return out_len;
}

int32_t SRTPSession::unprotectSRTCP(uint8_t *data, size_t len)
{
    if (len >= SRTP_MAX_BUFFER_SIZE)
    {
        return -1;
    }
    std::cout << "unprotect srtcp len:" << len << std::endl;
    int out_len = len;
    auto err = srtp_unprotect_rtcp(recv_ctx_, data, (int*)&out_len);
    if (err != srtp_err_status_ok)
    {
        std::cout << "decode srtcp failed, code:" << err << std::endl;
        return -2;
    }
    std::cout << "unprotect srtcp out len:" << out_len << std::endl;
    return out_len;
}
