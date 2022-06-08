#pragma once
#include <vector>
#include <string>
#include <string.h>

#include "dtls_define.h"
#include "dtls_handshake.h"
namespace mms
{
    struct ServerHelloDone : public HandShakeMsg
    {
        int32_t decode(uint8_t *data, size_t len);
        int32_t encode(uint8_t *data, size_t len);
        uint32_t size();
    };
};