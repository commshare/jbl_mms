#pragma once
#include <stdint.h>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace mms
{
    enum ContentType
    {
        change_cipher_spec = 20,
        alert = 21,
        handshake = 22,
        application_data = 23
    };

    struct ProtocolVersion
    {
        uint8_t major, minor;
        int32_t decode(uint8_t *data, size_t len);
        int32_t encode(uint8_t *data, size_t len);
        uint32_t size() {
            return 2;
        }
    };

    struct Random
    {
        uint32_t gmt_unix_time;
        uint8_t random_bytes[28];
        int32_t decode(uint8_t *data, size_t len);
        int32_t encode(uint8_t *data, size_t len);
        uint32_t size() {
            return 32;
        }
        void genRandom();
    };

    struct DtlsHeader
    {
        ContentType type;         /* same as TLSPlaintext.type */
        ProtocolVersion version;  /* same as TLSPlaintext.version */
        uint16_t epoch;           // New field
        uint64_t sequence_number; // New field
        uint16_t length;
        int32_t decode(uint8_t *data, size_t len);
    };

    struct DtlsMsg
    {
        virtual int32_t decode(uint8_t *data, size_t len) = 0;
        virtual int32_t encode(uint8_t *data, size_t len) = 0;
    };

    typedef uint16_t CipherSuite;
    // 加密套件定义
    #define TLS_RSA_EXPORT_WITH_RC4_40_MD5 0x0003
    #define TLS_RSA_WITH_RC4_128_MD5 0x0004
    #define TLS_RSA_WITH_RC4_128_SHA 0x0005
    #define TLS_DH_anon_EXPORT_WITH_RC4_40_MD5 0x0017
    #define TLS_DH_anon_WITH_RC4_128_MD5 0x0018
    #define TLS_KRB5_WITH_RC4_128_SHA 0x0020
    #define TLS_KRB5_WITH_RC4_128_MD5 0x0024
    #define TLS_KRB5_EXPORT_WITH_RC4_40_SHA 0x0028
    #define TLS_KRB5_EXPORT_WITH_RC4_40_MD5 0x002B
    #define TLS_PSK_WITH_RC4_128_SHA 0x008A
    #define TLS_DHE_PSK_WITH_RC4_128_SHA 0x008E
    #define TLS_RSA_PSK_WITH_RC4_128_SHA 0x0092
    #define TLS_ECDH_ECDSA_WITH_RC4_128_SHA 0xC002
    #define TLS_ECDHE_ECDSA_WITH_RC4_128_SHA 0xC007
    #define TLS_ECDH_RSA_WITH_RC4_128_SHA 0xC00C
    #define TLS_ECDHE_RSA_WITH_RC4_128_SHA 0xC011
    #define TLS_ECDH_anon_WITH_RC4_128_SHA 0xC016
    #define TLS_ECDHE_PSK_WITH_RC4_128_SHA 0xC033

    struct CipherSuites
    {
        std::vector<CipherSuite> cipher_suites;
        int32_t decode(uint8_t *data, size_t len);
    };

    typedef uint8_t CompressionMethod;
    #define COMPRESSION_METHOD_NULL 0x00
    struct CompressionMethods
    {
        std::vector<CompressionMethod> compression_methods;
        int32_t decode(uint8_t *data, size_t len);
    };

    struct DTLSCiphertext
    {
        DtlsHeader header;
        std::unique_ptr<DtlsMsg> msg;
        ContentType getType() const
        {
            return header.type;
        }
        int32_t decode(uint8_t *data, size_t len);
    };

    // @doc https://datatracker.ietf.org/doc/html/rfc5246#page-44
    //    The presence of extensions can be detected by
    //    determining whether there are bytes following the compression_methods
    //    at the end of the ClientHello.  Note that this method of detecting
    //    optional data differs from the normal TLS method of having a
    //    variable-length field, but it is used for compatibility with TLS
    //    before extensions were defined.
    enum ExtensionType
    {
        signature_algorithms = 0x000D,
        use_srtp = 0x000E
    };

    struct DtlsExtensionHeader
    {
        ExtensionType type;
        uint16_t length;
        int32_t decode(uint8_t *data, size_t len);
        int32_t encode(uint8_t *data, size_t len);
        uint32_t size() {
            return 4;
        }
    };

    struct DtlsExtItem
    {
        DtlsExtensionHeader header;
        ExtensionType getType() const
        {
            return header.type;
        }

        virtual int32_t decode(uint8_t *data, size_t len) = 0;
        virtual int32_t encode(uint8_t *data, size_t len) = 0;
        virtual uint32_t size() = 0;
    };

    struct UnknownExtItem : public DtlsExtItem
    {
        int32_t decode(uint8_t *d, size_t len);
        int32_t encode(uint8_t *d, size_t len);
        uint32_t size() {
            return 0;
        }
    };

    struct DtlsExtension
    {
        std::unordered_map<ExtensionType, std::unique_ptr<DtlsExtItem>> extensions;
        int32_t decode(uint8_t *data, size_t len);
        int32_t encode(uint8_t *data, size_t len);
        uint32_t size();

        DtlsExtItem *getExtension(ExtensionType t)
        {
            auto it = extensions.find(t);
            if (it == extensions.end())
            {
                return nullptr;
            }
            return it->second.get();
        }

        void addExtension(std::unique_ptr<DtlsExtItem> ext_item)
        {
            extensions.insert(std::pair(ext_item->getType(), std::move(ext_item)));
        }
    };
};