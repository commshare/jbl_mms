#pragma once
#include "srtp2/srtp.h"
#include "server/dtls/extension/dtls_use_srtp.h"

#define SRTP_MAX_BUFFER_SIZE 65535
namespace mms
{
    class SRTPSession
    {
    public:
        bool init(SRTPProtectionProfile policy, const std::string &recv_key, const std::string &send_key);
        int32_t unprotectSRTP(uint8_t *data, size_t len);
        int32_t unprotectSRTCP(uint8_t *data, size_t len);
        int32_t getSRTPOverhead() const 
        {
            return srtp_policy_.rtp.auth_tag_len;
        }

        int32_t getSRTCPOverhead() const
        {
            return srtp_policy_.rtcp.auth_tag_len;
        }
    private:
        srtp_t send_ctx_ = nullptr;
        srtp_t recv_ctx_ = nullptr;
        srtp_policy_t srtp_policy_;
    };
};