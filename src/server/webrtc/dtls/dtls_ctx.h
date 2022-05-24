#pragma once
#include "server/dtls/dtls_define.h"
namespace mms {
class DtlsCtx {
public:
    DtlsCtx() = default;
    ~DtlsCtx() {

    }

    bool processDtlsPacket(uint8_t *data, size_t len);
public:
    bool init();
private:
    bool processClientHello(DTLSCiphertext & msg);
};
};