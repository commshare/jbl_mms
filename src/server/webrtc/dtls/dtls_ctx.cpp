#include <iostream>

#include "server/dtls/dtls_define.h"
#include "dtls_ctx.h"
#include "server/dtls/dtls_define.h"

using namespace mms;

bool DtlsCtx::init()
{
    return true;
}

bool DtlsCtx::processDtlsPacket(uint8_t *data, size_t len)
{
    DTLSCiphertext dtls_msg;
    int32_t c = dtls_msg.decode(data, len);
    if (c < 0) {
        std::cout << "decode dtls msg failed, code:" << c << std::endl;
        return false;
    }
    return true;
}