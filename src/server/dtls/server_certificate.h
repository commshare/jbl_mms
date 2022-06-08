#pragma once
#include <vector>
#include <string>
#include <string.h>

#include "dtls_define.h"
#include "dtls_handshake.h"
namespace mms
{
    struct Certificate
    {
        uint32_t certificate_length;  // 24bit
        std::string certificate_asn1; // data

        Certificate(const std::string & cert) : certificate_asn1(cert) {

        }

        uint32_t size()
        {
            return 3 + certificate_asn1.size();
        }

        int32_t encode(uint8_t *data, size_t len)
        {
            uint8_t *data_start = data;
            if (len < 3)
            {
                return -1;
            }
            certificate_length = certificate_asn1.size();
            uint8_t *p = (uint8_t*)&certificate_length;
            data[0] = p[2];
            data[1] = p[1];
            data[2] = p[0];
            data += 3;
            len -= 3;

            if (len < certificate_length)
            {
                return -2;
            }
            memcpy(data, certificate_asn1.data(), certificate_length);
            data += certificate_length;
            return data - data_start;
        }

        int32_t decode(uint8_t *data, size_t len)
        {//todo implement it.
            return 0;
        }
    };

    struct ServerCertificate : public HandShakeMsg
    {
        uint32_t certificates_length; // 24bit
        std::vector<Certificate> certificates;

        int32_t decode(uint8_t *data, size_t len);
        int32_t encode(uint8_t *data, size_t len);
        uint32_t size();

        void addCert(const std::string & cert) {
            certificates.push_back(Certificate(cert));
        }
    };
};