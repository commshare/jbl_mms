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
        
    private:
        X509 *certificate_ = nullptr;
        std::string finger_print_;
        static DtlsCert instance_;
    private:
        bool createCert();
    };
};