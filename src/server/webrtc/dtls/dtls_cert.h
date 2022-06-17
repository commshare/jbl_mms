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
        bool init(const std::string & domain);

        const std::string &getFingerPrint() const
        {
            return finger_print_;
        }

        const std::string &getDer() const {
            return der_;
        }

        RSA *getRSA() {
            return rsa_;
        }
    private:
        std::string domain_;
        X509 *certificate_ = nullptr;
        RSA *rsa_ = nullptr;
        EVP_PKEY *pkey_ = nullptr;
        std::string finger_print_;
        std::string der_;
    private:
        bool createCert();
    };
};