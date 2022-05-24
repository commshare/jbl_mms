#pragma once
#include "dtls_define.h"
#include "dtls_handshake.h"
namespace mms {
    struct HelloVerifyRequest : public DtlsMsg
    {
        ProtocolVersion server_version;
        std::string cookie;
        int32_t decode(uint8_t *data, size_t len);
    };
};