#pragma once
#include <string>
#include <memory>

#include "openssl/bio.h"
#include "openssl/ssl.h"
#include "openssl/x509.h"

namespace mms
{
    class DtlsCert
    {
    public:
        DtlsCert() = default;
        ~DtlsCert();

        static DtlsCert *getInstance()
        {
            return &instance_;
        }

        bool init();

        const std::string &getFingerPrint() const
        {
            return finger_print_;
        }
        
        SSL_CTX *getSSLCtx();

    private:
        std::string sign_method_ = "sha256";
        X509 *certificate_ = nullptr;
        std::string finger_print_;
        EVP_PKEY *private_key_ = nullptr;
        SSL_CTX *ssl_ctx_ = nullptr;
        static DtlsCert instance_;

    private:
        bool createCert();
        bool initSSLCtx();
        static int sslVerifyCallback(int preverify_ok, X509_STORE_CTX *ctx);
        static void sslInfo(const SSL *ssl, int where, int ret);
    };
};