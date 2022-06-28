#pragma once
#include "dtls_define.h"
#include "dtls_handshake.h"

#include "base/utils/utils.h"
namespace mms {
    struct HelloVerifyRequest : public HandShakeMsg
    {
        DtlsProtocolVersion server_version;
        std::string cookie;
        int32_t decode(uint8_t *data, size_t len);
        uint32_t size();
        int32_t encode(uint8_t *data, size_t len);
        void setDtlsProtocolVersion(const DtlsProtocolVersion &pv)
        {
            server_version = pv;
        }

        const DtlsProtocolVersion &getDtlsProtocolVersion() const
        {
            return server_version;
        }

        void genCookie()//todo Cookie = HMAC(Secret, Client-IP, Client-Parameters)
        {
            cookie = Utils::randStr(12);
        }
    };
};