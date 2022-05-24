#pragma once
#include "dtls_define.h"
#include "dtls_handshake.h"
namespace mms
{
    struct ClientHello : public HandShakeMsg
    {
        ProtocolVersion client_version;
        Random random;
        std::string session_id;
        std::string cookie; // 0-32
        CipherSuites cipher_suites;
        CompressionMethods compression_methods;
        // 1.2版本才有
        DtlsExtension extension;

        int32_t decode(uint8_t *data, size_t len);
        int32_t encode(uint8_t *data, size_t len);
    };
};