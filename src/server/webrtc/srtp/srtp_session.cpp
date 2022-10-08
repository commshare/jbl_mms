#include <string.h>
#include "srtp_session.h"
using namespace mms;

bool SRTPSession::init(SRTPProtectionProfile profile, const std::string &recv_key, const std::string &send_key)
{
    memset(&srtp_policy_, 0, sizeof(srtp_policy_t));
    switch (profile)
    {
    case SRTP_AES128_CM_HMAC_SHA1_80:
        srtp_crypto_policy_set_aes_cm_128_hmac_sha1_80(&srtp_policy_.rtp);
        srtp_crypto_policy_set_aes_cm_128_hmac_sha1_80(&srtp_policy_.rtcp);
        break;
    default:
        break;
    }

    if (recv_key.size() != srtp_policy_.rtp.cipher_key_len) {
        std::cout << "recv_key was not match, len:" << recv_key.size() 
                << ", expected:" << srtp_policy_.rtp.cipher_key_len << std::endl;
    }
    
    if (send_key.size() != srtp_policy_.rtp.cipher_key_len) {
        std::cout << "send_key was not match, len:" << send_key.size() 
                << ", expected:" << srtp_policy_.rtp.cipher_key_len << std::endl;
    };


    srtp_policy_.ssrc.value = 0;
    srtp_policy_.allow_repeat_tx = 1;
    // srtp_policy.window_size = 1024;
    srtp_policy_.window_size = 4096;
    srtp_policy_.next = NULL;

    srtp_policy_.ssrc.type = ssrc_any_inbound;
    srtp_policy_.key = (unsigned char *)(recv_key.c_str());
    auto err = srtp_create(&recv_ctx_, &srtp_policy_);
    if (err != srtp_err_status_ok)
    {
        return false;
    }

    srtp_policy_.ssrc.type = ssrc_any_outbound;
    srtp_policy_.key = (unsigned char *)(send_key.c_str());
    err = srtp_create(&send_ctx_, &srtp_policy_);
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

    int out_len = len;
    auto err = srtp_unprotect(recv_ctx_, data, (int*)&out_len);
    if (err != srtp_err_status_ok)
    {
        return -2;
    }
    return out_len;
}

int32_t SRTPSession::unprotectSRTCP(uint8_t *data, size_t len)
{
    if (len >= SRTP_MAX_BUFFER_SIZE)
    {
        return -1;
    }

    int out_len = len;
    auto err = srtp_unprotect_rtcp(recv_ctx_, data, (int*)&out_len);
    if (err != srtp_err_status_ok)
    {
        std::cout << "decode srtcp failed, code:" << err << std::endl;
        return -2;
    }
    return out_len;
}
