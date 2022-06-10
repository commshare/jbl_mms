#include <iostream>

#include "server/dtls/dtls_handshake.h"
#include "server/dtls/server_hello.h"
#include "server/dtls/client_hello.h"
#include "server/dtls/extension/dtls_use_srtp.h"

#include "server/dtls/server_certificate.h"
#include "server/dtls/server_hello_done.h"
#include "dtls_ctx.h"
#include "dtls_cert.h"
#include "server/dtls/tls_prf.h"

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
        else if (handshake->getType() == client_key_exchange)
        {
            return processClientKeyExchange(dtls_msg, sock, remote_ep, yield);
        }
    }

    return true;
}

bool DtlsCtx::processClientHello(DTLSCiphertext & recv_msg, UdpSocket *sock, const boost::asio::ip::udp::endpoint &remote_ep, boost::asio::yield_context & yield)
{
    client_hello_ = recv_msg;

    HandShake * recv_handshake_msg = (HandShake *)client_hello_.value().msg.get();
    ClientHello *client_hello = (ClientHello *)recv_handshake_msg->msg.get();
    {// send server hello
        DTLSCiphertext resp_msg;
        resp_msg.setType(handshake);
        resp_msg.setDtlsProtocolVersion(DtlsProtocolVersion(DTLS_MAJOR_VERSION1, DTLS_MINOR_VERSION2));
        resp_msg.setSequenceNo(message_seq_);

        std::unique_ptr<HandShake> resp_handshake = std::unique_ptr<HandShake>(new HandShake);
        auto *s = new ServerHello;
        std::unique_ptr<HandShakeMsg> resp_server_hello = std::unique_ptr<HandShakeMsg>(s);
        std::unique_ptr<UseSRtpExt> use_srtp_ext = std::unique_ptr<UseSRtpExt>(new UseSRtpExt);
        use_srtp_ext->addProfile(SRTP_AES128_CM_HMAC_SHA1_80);
        s->addExtension(std::move(use_srtp_ext));
        s->setDtlsProtocolVersion(DtlsProtocolVersion(DTLS_MAJOR_VERSION1, DTLS_MINOR_VERSION2));
        s->genRandom();
        s->setCipherSuite(TLS_RSA_WITH_AES_128_CBC_SHA);
        resp_handshake->setType(server_hello);
        resp_handshake->setMsg(std::move(resp_server_hello));
        resp_handshake->setMessageSeq(message_seq_);
        resp_msg.setMsg(std::move(resp_handshake));
        auto resp_size = resp_msg.size();

        std::unique_ptr<uint8_t[]> data = std::unique_ptr<uint8_t[]>(new uint8_t[resp_size]);
        int32_t consumed = resp_msg.encode(data.get(), resp_size);
        if (consumed < 0) 
        {// todo:add log
            return false;
        }
        sock->sendTo(std::move(data), resp_size, remote_ep, yield);
        message_seq_++;
    }

    {
        DTLSCiphertext resp_msg;
        resp_msg.setType(handshake);
        resp_msg.setDtlsProtocolVersion(DtlsProtocolVersion(DTLS_MAJOR_VERSION1, DTLS_MINOR_VERSION2));
        resp_msg.setSequenceNo(message_seq_);

        std::unique_ptr<HandShake> resp_handshake = std::unique_ptr<HandShake>(new HandShake);
        resp_handshake->setType(certificate);
        auto *s = new ServerCertificate;
        std::unique_ptr<HandShakeMsg> resp_server_certificate = std::unique_ptr<HandShakeMsg>(s);
        s->addCert(DtlsCert::getInstance()->getDer());
        resp_handshake->setMsg(std::move(resp_server_certificate));
        resp_handshake->setMessageSeq(message_seq_);
        resp_msg.setMsg(std::move(resp_handshake));
        auto resp_size = resp_msg.size();

        std::unique_ptr<uint8_t[]> data = std::unique_ptr<uint8_t[]>(new uint8_t[resp_size]);
        int32_t consumed = resp_msg.encode(data.get(), resp_size);
        if (consumed < 0) 
        {// todo:add log
            return false;
        }
        sock->sendTo(std::move(data), resp_size, remote_ep, yield);
        message_seq_++;
    }

    {
        DTLSCiphertext resp_msg;
        resp_msg.setType(handshake);
        resp_msg.setDtlsProtocolVersion(DtlsProtocolVersion(DTLS_MAJOR_VERSION1, DTLS_MINOR_VERSION2));
        resp_msg.setSequenceNo(message_seq_);

        std::unique_ptr<HandShake> resp_handshake = std::unique_ptr<HandShake>(new HandShake);
        resp_handshake->setType(server_hello_done);
        auto *s = new ServerHelloDone;
        std::unique_ptr<HandShakeMsg> resp_server_hello_done = std::unique_ptr<HandShakeMsg>(s);
        resp_handshake->setMsg(std::move(resp_server_hello_done));
        resp_handshake->setMessageSeq(message_seq_);
        resp_msg.setMsg(std::move(resp_handshake));
        auto resp_size = resp_msg.size();

        std::unique_ptr<uint8_t[]> data = std::unique_ptr<uint8_t[]>(new uint8_t[resp_size]);
        int32_t consumed = resp_msg.encode(data.get(), resp_size);
        if (consumed < 0) 
        {// todo:add log
            return false;
        }
        sock->sendTo(std::move(data), resp_size, remote_ep, yield);
        message_seq_++;
        server_hello_ = resp_msg;
    }

    return true;
}

bool DtlsCtx::processClientKeyExchange(DTLSCiphertext & msg, UdpSocket *sock, const boost::asio::ip::udp::endpoint &remote_ep, boost::asio::yield_context & yield)
{
    HandShake * recv_handshake_msg = (HandShake *)msg.msg.get();
    ClientKeyExchange *client_key_exchange_msg = (ClientKeyExchange *)recv_handshake_msg->msg.get();
    std::string decoded_data;
    decoded_data.resize(1024);
    int n = decryptRSA(client_key_exchange_msg->enc_pre_master_secret.pre_master_secret, decoded_data);
    if (n < 0)
    {
        return false;
    }
    int consumed = pre_master_secret_.decode((uint8_t*)decoded_data.data(), n);
    if (consumed < 0)
    {
        return -1;
    }
    HandShake * recv_client_hello_msg = (HandShake *)client_hello_.value().msg.get();
    ClientHello *client_hello = (ClientHello *)recv_client_hello_msg->msg.get();
    if (client_hello->client_version != pre_master_secret_.client_version)
    {
        return false;
    }
    // 生成master secret
    HandShake * send_server_hello_msg = (HandShake *)server_hello_.value().msg.get();
    ServerHello *server_hello = (ServerHello *)send_server_hello_msg->msg.get();
    std::string seed;
    seed.append((char*)client_hello->random.random_raw, 32);
    seed.append((char*)server_hello->random.random_raw, 32);
    master_secret_ = PRF(decoded_data, "master secret", seed, 48);
    memcpy(security_params_.master_secret, master_secret_.data(), 48);
    memcpy(security_params_.client_random, client_hello->random.random_raw, 32);
    memcpy(security_params_.server_random, server_hello->random.random_raw, 32);
    // 生成key material
    // @doc rfc5764 4.1.2.  SRTP Protection Profiles
    //     SRTP_AES128_CM_HMAC_SHA1_80
    //          cipher: AES_128_CM
    //          cipher_key_length: 128
    //          cipher_salt_length: 112
    //          maximum_lifetime: 2^31
    //          auth_function: HMAC-SHA1
    //          auth_key_length: 160
    //          auth_tag_length: 80
    //    SRTP_AES128_CM_HMAC_SHA1_32
    //          cipher: AES_128_CM
    //          cipher_key_length: 128
    //          cipher_salt_length: 112
    //          maximum_lifetime: 2^31
    //          auth_function: HMAC-SHA1
    //          auth_key_length: 160
    //          auth_tag_length: 32
    //          RTCP auth_tag_length: 80
    //    SRTP_NULL_HMAC_SHA1_80
    //          cipher: NULL
    //          cipher_key_length: 0
    //          cipher_salt_length: 0
    //          maximum_lifetime: 2^31
    //          auth_function: HMAC-SHA1
    //          auth_key_length: 160
    //          auth_tag_length: 80

    size_t srtp_cipher_key_length = 16;//128/8
    size_t srtp_cipher_salt_length = 14;//112/8
    // total:(16+14)*2
    size_t total_key_material_need = (srtp_cipher_key_length + srtp_cipher_salt_length)*2;
    std::string srtp_key_block = PRF(master_secret_, "EXTRACTOR-dtls_srtp", seed, total_key_material_need);

    size_t offset = 0;
    std::string client_master_key((char*)(srtp_key_block.data()), srtp_cipher_key_length);
    offset += srtp_cipher_key_length;
    std::string server_master_key((char*)(srtp_key_block.data() + offset), srtp_cipher_key_length);
    offset += srtp_cipher_key_length;
    std::string client_master_salt((char*)(srtp_key_block.data() + offset), srtp_cipher_salt_length);
    offset += srtp_cipher_salt_length;
    std::string server_master_salt((char*)(srtp_key_block.data() + offset), srtp_cipher_salt_length);

    recv_key_ = client_master_key + client_master_salt;
    send_key_ = server_master_key + server_master_salt;
    
    // printf("srtp key:");
    // for (int pos = 0; pos < total_key_material_need; pos++) {
    //     printf("%02x", (uint8_t)srtp_key_block[pos]);
    // }
    // printf("\r\n");
    
    return true;
}

int32_t DtlsCtx::decryptRSA(const std::string & enc_data, std::string & dec_data)
{
    int num = RSA_private_decrypt(RSA_size(DtlsCert::getInstance()->getRSA()), (const uint8_t*)enc_data.data(), (uint8_t*)dec_data.data(), DtlsCert::getInstance()->getRSA(), RSA_PKCS1_PADDING);
    return num;
}

bool DtlsCtx::calcMasterSecret()
{
    
}