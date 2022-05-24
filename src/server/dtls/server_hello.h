#pragma once
#include "dtls_define.h"
#include "dtls_handshake.h"
namespace mms
{
    struct ServerHello : public HandShakeMsg
    {
        DtlsProtocolVersion server_version;
        Random random;
        std::string session_id;
        CipherSuite cipher_suite;
        CompressionMethod compression_method = COMPRESSION_METHOD_NULL;
        // 1.2版本才有
        DtlsExtension extension;

        int32_t decode(uint8_t *data, size_t len);
        int32_t encode(uint8_t *data, size_t len);
        uint32_t size();

        void setDtlsProtocolVersion(const DtlsProtocolVersion &pv)
        {
            server_version = pv;
        }

        const DtlsProtocolVersion &getDtlsProtocolVersion() const
        {
            return server_version;
        }

        void genRandom();

        const Random &getRandom() const
        {
            return random;
        }

        const std::string &getSessionId() const
        {
            return session_id;
        }

        void setSessionId(const std::string &val)
        {
            session_id = val;
        }

        const CipherSuite &getCipherSuite() const
        {
            return cipher_suite;
        }

        void setCipherSuite(const CipherSuite &val)
        {
            cipher_suite = val;
        }

        const DtlsExtension & getDtlsExtension() const 
        {
            return extension;
        }

        void addExtension(std::unique_ptr<DtlsExtItem> ext_item)
        {
            extension.addExtension(std::move(ext_item));
        }
    };
};