#include <iostream>

#include "server/dtls/dtls_handshake.h"
#include "server/dtls/server_hello.h"
#include "server/dtls/client_hello.h"
#include "dtls_ctx.h"

using namespace mms;

bool DtlsCtx::init()
{
    return true;
}

bool DtlsCtx::processDtlsPacket(uint8_t *data, size_t len)
{
    DTLSCiphertext dtls_msg;
    int32_t c = dtls_msg.decode(data, len);
    if (c < 0)
    {
        return false;
    }

    if (dtls_msg.getType() == handshake)
    {
        HandShake *handshake = (HandShake *)dtls_msg.msg.get();
        if (handshake->getType() == client_hello)
        {
            return processClientHello(dtls_msg);
        }
    }

    return true;
}

bool DtlsCtx::processClientHello(DTLSCiphertext & recv_msg)
{
    client_hello_ = recv_msg;

    HandShake * handshake_msg = (HandShake *)client_hello_.value().msg.get();
    ClientHello *client_hello = (ClientHello *)handshake_msg->msg.get();
    DTLSCiphertext resp_msg;
    resp_msg.setType(handshake);
    resp_msg.setDtlsProtocolVersion(DtlsProtocolVersion(DTLS_MAJOR_VERSION1, DTLS_MINOR_VERSION1));

    std::unique_ptr<HandShake> handshake = std::unique_ptr<HandShake>(new HandShake);
    auto *s = new ServerHello;
    std::unique_ptr<HandShakeMsg> server_hello = std::unique_ptr<HandShakeMsg>(s);
    s->setDtlsProtocolVersion(DtlsProtocolVersion(DTLS_MAJOR_VERSION1, DTLS_MINOR_VERSION2));
    s->genRandom();
    s->setCipherSuite(TLS_RSA_WITH_AES_128_CBC_SHA);
    



    handshake->setMsg(std::move(server_hello));
    resp_msg.setMsg(std::move(handshake));
    return true;
}