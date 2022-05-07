#pragma once
#include <string>
#include "openssl/bio.h"
#include "openssl/ssl.h"
#include "openssl/x509.h"

namespace mms {
class DtlsCert {
public:
    DtlsCert() = default;
    ~DtlsCert() {

    }

    static DtlsCert * getInstance() {
        return &instance_;
    }

    bool init(const std::string & sha_method);

    SSL_CTX *getSSLCtx();
private:
    std::string sign_method_ = "sha256";
    X509        *certificate_ = nullptr;
    EVP_PKEY    *private_key_ = nullptr;
    SSL_CTX     *ssl_ctx_ = nullptr;
    static DtlsCert instance_;
private:
    bool createCert();
    bool initSSLCtx();
    static int sslVerifyCallback(int preverify_ok, X509_STORE_CTX *ctx);
    static void sslInfo(const SSL* ssl, int where, int ret);
};
};