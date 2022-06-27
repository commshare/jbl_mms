#pragma once
#include "dtls_define.h"
#include "dtls_handshake.h"
namespace mms
{
    struct DtlsFinished : public HandShakeMsg
    {
        DtlsFinished() = default;
        std::string verify_data;

        int32_t decode(uint8_t *data, size_t len);
        int32_t encode(uint8_t *data, size_t len);
        uint32_t size() {
            return 12;
        }

        std::string & getVerifyData()
        {
            return verify_data;
        }
    };
};