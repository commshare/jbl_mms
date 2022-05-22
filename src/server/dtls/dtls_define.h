#pragma once
#include <stdint.h>
#include <string>
#include <vector>

namespace mms
{
    enum ContentType
    {
        change_cipher_spec = 20,
        alert = 21,
        handshake = 22,
        application_data = 23
    };

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

    typedef struct ProtocolVersion
    {
        uint8_t major, minor;
    };

    typedef struct Random
    {
        uint32_t gmt_unix_time;
        uint8_t random_bytes[28];
    };

    typedef uint16_t CipherSuite;
    typedef uint8_t CompressionMethod;
    typedef struct
    {
        ProtocolVersion client_version;
        Random random;
        std::string session_id;
        std::string cookie; // 0-32
        std::vector<CipherSuite> cipher_suites;
        std::vector<CompressionMethod> compression_methods;
    } ClientHello;

    typedef struct
    {
        ProtocolVersion server_version;
        std::string cookie;
    } HelloVerifyRequest;

    typedef struct
    {
        ContentType type;
        ProtocolVersion version;
        uint16_t epoch;
        uint64_t sequence_number; //只用48位
        uint16_t length;
        std::string fragment;
    } DTLSPlaintext;

    typedef struct
    {
        ContentType type;         /* same as TLSPlaintext.type */
        ProtocolVersion version;  /* same as TLSPlaintext.version */
        uint16_t epoch;           // New field
        uint64_t sequence_number; // New field
        uint16_t length;
        std::string fragment;
    } DTLSCompressed;

    typedef struct
    {
        ContentType type;
        ProtocolVersion version;
        uint16_t epoch;           // New field
        uint64_t sequence_number; // New field
        uint16_t length;
        std::string fragment;
    } DTLSCiphertext;

};