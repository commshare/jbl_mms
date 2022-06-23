#pragma once
#include "dtls_define.h"
#include "dtls_handshake.h"
namespace mms
{
    struct ClientHello : public HandShakeMsg
    {
        ClientHello() = default;
        DtlsProtocolVersion client_version;
        Random random;
        std::string session_id;
        std::string cookie; // 0-32
        CipherSuites cipher_suites;
        CompressionMethods compression_methods;
        // 1.2版本才有
        DtlsExtension extension;

        int32_t decode(uint8_t *data, size_t len);
        int32_t encode(uint8_t *data, size_t len);
        uint32_t size() {
            uint32_t s = 0;
            s += client_version.size();
            s += random.size();
            s += session_id.size() + 1;
            s += cookie.size() + 1;
            s += cipher_suites.size();
            s += compression_methods.size();
            s += extension.size();
            return s;
        }

        CipherSuites & getCiperSuites()
        {
            return cipher_suites;
        }
    };
};