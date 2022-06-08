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

bool DtlsCtx::processDtlsPacket(uint8_t *data, size_t len, UdpSocket *sock, const boost::asio::ip::udp::endpoint &remote_ep, boost::asio::yield_context & yield)
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
            return processClientHello(dtls_msg, sock, remote_ep, yield);
        }
    }

    return true;
}

bool DtlsCtx::processClientHello(DTLSCiphertext & recv_msg, UdpSocket *sock, const boost::asio::ip::udp::endpoint &remote_ep, boost::asio::yield_context & yield)
{
    client_hello_ = recv_msg;

    HandShake * recv_handshake_msg = (HandShake *)client_hello_.value().msg.get();
    ClientHello *client_hello = (ClientHello *)recv_handshake_msg->msg.get();

    DTLSCiphertext resp_msg;
    resp_msg.setType(handshake);
    resp_msg.setDtlsProtocolVersion(DtlsProtocolVersion(DTLS_MAJOR_VERSION1, DTLS_MINOR_VERSION0));

    std::unique_ptr<HandShake> resp_handshake = std::unique_ptr<HandShake>(new HandShake);
    auto *s = new ServerHello;
    std::unique_ptr<HandShakeMsg> resp_server_hello = std::unique_ptr<HandShakeMsg>(s);
    s->setDtlsProtocolVersion(DtlsProtocolVersion(DTLS_MAJOR_VERSION1, DTLS_MINOR_VERSION2));
    s->genRandom();
    s->setCipherSuite(TLS_RSA_WITH_AES_128_CBC_SHA);
    resp_handshake->setType(server_hello);
    resp_handshake->setMsg(std::move(resp_server_hello));
    resp_msg.setMsg(std::move(resp_handshake));
    auto resp_size = resp_msg.size();

    std::unique_ptr<uint8_t[]> data = std::unique_ptr<uint8_t[]>(new uint8_t[resp_size]);
    int32_t consumed = resp_msg.encode(data.get(), resp_size);
    if (consumed < 0) 
    {// todo:add log
        std::cout << "************************* encode failed, consumed:" << consumed << " resp_size:" << resp_size << " ******************" << std::endl;
        return false;
    } 
    else 
    {
        std::cout << "************************ encode consumed:" << consumed << ", resp_size:" << resp_size << " *******************" << std::endl;
    }

    {
        DTLSCiphertext server_hello_msg;
        int32_t c = server_hello_msg.decode(data.get(), resp_size);
        if (c < 0)
        {
            return false;
        }

        if (server_hello_msg.getType() == handshake)
        {
            HandShake *handshake = (HandShake *)server_hello_msg.msg.get();
            if (handshake->getType() == server_hello)
            {
                std::cout << "decode server hello succeed." << std::endl;
            }
        }

        printf("\r\n");
        printf("\r\n");
        for(size_t i = 1; i <= resp_size; i++) 
        {
            printf("%02x ", data.get()[i-1]);
            if (i % 16 == 0) {
                printf("\r\n");
            }
        }
        printf("\r\n");
    }

    sock->sendTo(std::move(data), resp_size, remote_ep, yield);

    

    return true;
}