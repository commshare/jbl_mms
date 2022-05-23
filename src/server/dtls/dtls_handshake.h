#pragma once
#include <string>
#include <vector>
#include <stdint.h>

#include "dtls_define.h"
#include "dtls_msg.h"

namespace mms
{
    enum HandshakeType
    {
        hello_request = 0,
        client_hello = 1,
        server_hello = 2,
        hello_verify_request = 3, // New field
        certificate = 11,
        server_key_exchange = 12,
        certificate_request = 13,
        server_hello_done = 14,
        certificate_verify = 15,
        client_key_exchange = 16,
        finished = 20
    };

    struct HandShakeMsg
    {
        virtual int32_t decode(uint8_t *data, size_t len) = 0;
    };

    struct HandShake : public DtlsMsg
    {
        HandshakeType msg_type; /* handshake type */
        uint32_t length;        /* bytes in message (24bit) */
        std::unique_ptr<HandShakeMsg> msg;
        int32_t decode(uint8_t *data, size_t len);
    };

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
        // std::vector<std::unique_ptr

        int32_t decode(uint8_t *data, size_t len);
    };

    struct HelloVerifyRequest : public DtlsMsg
    {
        ProtocolVersion server_version;
        std::string cookie;
        int32_t decode(uint8_t *data, size_t len);
    };

};