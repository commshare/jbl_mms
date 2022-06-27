#include <iostream>

#include "server/dtls/dtls_handshake.h"
#include "server/dtls/server_hello.h"
#include "server/dtls/client_hello.h"
#include "server/dtls/extension/dtls_use_srtp.h"

#include "server/dtls/server_certificate.h"
#include "server/dtls/server_hello_done.h"
#include "server/dtls/change_cipher_spec.h"
#include "server/dtls/finished.h"

#include "dtls_session.h"
#include "dtls_cert.h"
#include "server/dtls/tls_prf.h"

#include "base/utils/utils.h"
#include "openssl/aes.h"
using namespace mms;
// tls1.2参考：https://tls12.ulfheim.net/
bool DtlsSession::init()
{
    next_msg_require_handler_ = std::bind(&DtlsSession::requireClientHello, this);
    next_msg_handler_ = std::bind(&DtlsSession::processClientHello, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
    return true;
}

void DtlsSession::setDtlsCert(std::shared_ptr<DtlsCert> cert)
{
    dtls_cert_ = cert;
}

int32_t DtlsSession::decryptRSA(const std::string &enc_data, std::string &dec_data)
{
    int num = RSA_private_decrypt(RSA_size(dtls_cert_->getRSA()), (const uint8_t *)enc_data.data(), (uint8_t *)dec_data.data(), dtls_cert_->getRSA(), RSA_PKCS1_PADDING);
    return num;
}

bool DtlsSession::calcMasterSecret()
{
    return false;
}

bool DtlsSession::processDtlsPacket(uint8_t *data, size_t len, UdpSocket *sock, const boost::asio::ip::udp::endpoint &remote_ep, boost::asio::yield_context &yield)
{
    int32_t consumed = 0;
    std::shared_ptr<DTLSPlaintext> dtls_msg;
    while (len > 0)
    {
        uint16_t epoch = ntohs(*(uint16_t *)(data + DTLS_EPOCH_OFFSET));
        if (epoch == 0)
        {
            dtls_msg = std::make_shared<DTLSPlaintext>();
            consumed = dtls_msg->decode(data, len);
            if (consumed < 0)
            {
                return false;
            }
            
            if (dtls_msg->getType() == handshake)
            {
                HandshakeType typ;
                HandShake * h = (HandShake*)dtls_msg->msg.get();
                typ = h->getType();
                
                if (!recv_handshake_map_[typ])
                {
                    handshake_data_.append((char*)data + DTLS_HEADER_SIZE, consumed - DTLS_HEADER_SIZE);
                    recv_handshake_map_[typ] = true;
                }
            }

            data += consumed;
            len -= consumed;
            uint64_t next_receive_seq = epoch_receive_seq_map_[dtls_msg->getEpoch()];

            if (dtls_msg->getSequenceNo() < next_receive_seq)
            {//discard
                continue;
            }
            else if (dtls_msg->getSequenceNo() ==  next_receive_seq)
            {//process
                if (!next_msg_handler_(dtls_msg, sock, remote_ep, yield))
                {
                    return false;
                }
            }
            else
            {//queue
                unhandled_msgs_.insert(std::pair(dtls_msg->getSequenceNo(), dtls_msg));
            }
            
            // 处理未处理消息
            if (!ciper_state_changed_) 
            {
                bool process_next = true;
                do {
                    dtls_msg = next_msg_require_handler_();
                    if (!dtls_msg) 
                    {
                        break;
                    }
                    process_next = next_msg_handler_(dtls_msg, sock, remote_ep, yield);
                } while (process_next);
            }
        }
        else
        {
            recv_finished_msg_ = std::make_shared<DTLSCiperText>();
            if (!ciper_suite_)
            {
                return false;
            }
            
            consumed = recv_finished_msg_->decode(data, len, ciper_suite_.get());
            data += consumed;
            len -= consumed;
            if (!processHandShakeFinished(recv_finished_msg_, sock, remote_ep, yield))
            {
                return false;
            }

            if (!recv_handshake_map_[finished])
            {
                handshake_data_.append((char*)data + DTLS_HEADER_SIZE, consumed - DTLS_HEADER_SIZE);
                recv_handshake_map_[finished] = true;
            }
        }
    }
    
    return true;
}

std::shared_ptr<DTLSPlaintext> DtlsSession::requireClientHello()
{
    std::shared_ptr<DTLSPlaintext> dtls_msg;
    std::map<uint64_t, std::shared_ptr<DTLSPlaintext>>::iterator it;
    for (it = unhandled_msgs_.begin(); it != unhandled_msgs_.end(); it++)
    {
        if (it->second->getType() != handshake)
        {
            continue;
        }

        HandShake *handshake = (HandShake *)it->second->msg.get();
        if (handshake->getType() != client_hello)
        {
            continue;
        }

        dtls_msg = it->second;
        unhandled_msgs_.erase(it);
        return dtls_msg;
    }
    return nullptr;
}

bool DtlsSession::processClientHello(std::shared_ptr<DTLSPlaintext> recv_msg, UdpSocket *sock, const boost::asio::ip::udp::endpoint &remote_ep, boost::asio::yield_context &yield)
{
    client_hello_ = recv_msg;

    // HandShake *recv_handshake_msg = (HandShake *)client_hello_->msg.get();
    // ClientHello *client_hello = (ClientHello *)recv_handshake_msg->msg.get();
    { // send server hello
        std::shared_ptr<DTLSPlaintext> resp_msg = std::make_shared<DTLSPlaintext>();
        resp_msg->setType(handshake);
        resp_msg->setDtlsProtocolVersion(DtlsProtocolVersion(DTLS_MAJOR_VERSION1, DTLS_MINOR_VERSION2));
        resp_msg->setSequenceNo(epoch_send_seq_map_[0]);

        std::unique_ptr<HandShake> resp_handshake = std::unique_ptr<HandShake>(new HandShake);
        auto *s = new ServerHello;
        std::unique_ptr<HandShakeMsg> resp_server_hello = std::unique_ptr<HandShakeMsg>(s);
        std::unique_ptr<UseSRtpExt> use_srtp_ext = std::unique_ptr<UseSRtpExt>(new UseSRtpExt);
        use_srtp_ext->addProfile(SRTP_AES128_CM_HMAC_SHA1_80);
        s->addExtension(std::move(use_srtp_ext));
        s->setDtlsProtocolVersion(DtlsProtocolVersion(DTLS_MAJOR_VERSION1, DTLS_MINOR_VERSION2));
        s->genRandom();
        //todo 遍历client hello中的suites，按优先级选择
        s->setCipherSuite(TLS_RSA_WITH_AES_128_CBC_SHA);
        ciper_suite_ = std::unique_ptr<DtlsCiperSuite>(new RSA_AES128_SHA1_Cipher);

        resp_handshake->setType(server_hello);
        resp_handshake->setMsg(std::move(resp_server_hello));
        resp_handshake->setMessageSeq(epoch_send_seq_map_[0]);
        resp_msg->setMsg(std::move(resp_handshake));
        auto resp_size = resp_msg->size();

        std::unique_ptr<uint8_t[]> data = std::unique_ptr<uint8_t[]>(new uint8_t[resp_size]);
        int32_t consumed = resp_msg->encode(data.get(), resp_size);
        if (consumed < 0)
        { // todo:add log
            return false;
        }
        handshake_data_.append((char*)data.get() + DTLS_HEADER_SIZE, resp_size - DTLS_HEADER_SIZE);
        sock->sendTo(std::move(data), resp_size, remote_ep, yield);
        epoch_send_seq_map_[0]++;
        server_hello_ = resp_msg;
        std::cout << "append handshake data" << std::endl;
    }

    {
        std::shared_ptr<DTLSPlaintext> resp_msg = std::make_shared<DTLSPlaintext>();
        resp_msg->setType(handshake);
        resp_msg->setDtlsProtocolVersion(DtlsProtocolVersion(DTLS_MAJOR_VERSION1, DTLS_MINOR_VERSION2));
        resp_msg->setSequenceNo(epoch_send_seq_map_[0]);

        std::unique_ptr<HandShake> resp_handshake = std::unique_ptr<HandShake>(new HandShake);
        resp_handshake->setType(certificate);
        auto *s = new ServerCertificate;
        std::unique_ptr<HandShakeMsg> resp_server_certificate = std::unique_ptr<HandShakeMsg>(s);
        s->addCert(dtls_cert_->getDer());
        resp_handshake->setMsg(std::move(resp_server_certificate));
        resp_handshake->setMessageSeq(epoch_send_seq_map_[0]);
        resp_msg->setMsg(std::move(resp_handshake));
        auto resp_size = resp_msg->size();

        std::unique_ptr<uint8_t[]> data = std::unique_ptr<uint8_t[]>(new uint8_t[resp_size]);
        int32_t consumed = resp_msg->encode(data.get(), resp_size);
        if (consumed < 0)
        { // todo:add log
            return false;
        }
        handshake_data_.append((char*)data.get() + DTLS_HEADER_SIZE, resp_size - DTLS_HEADER_SIZE);
        sock->sendTo(std::move(data), resp_size, remote_ep, yield);
        epoch_send_seq_map_[0]++;
        std::cout << "append handshake data" << std::endl;
    }

    {
        std::shared_ptr<DTLSPlaintext> resp_msg = std::make_shared<DTLSPlaintext>();
        resp_msg->setType(handshake);
        resp_msg->setDtlsProtocolVersion(DtlsProtocolVersion(DTLS_MAJOR_VERSION1, DTLS_MINOR_VERSION2));
        resp_msg->setSequenceNo(epoch_send_seq_map_[0]);

        std::unique_ptr<HandShake> resp_handshake = std::unique_ptr<HandShake>(new HandShake);
        resp_handshake->setType(server_hello_done);
        auto *s = new ServerHelloDone;
        std::unique_ptr<HandShakeMsg> resp_server_hello_done = std::unique_ptr<HandShakeMsg>(s);
        resp_handshake->setMsg(std::move(resp_server_hello_done));
        resp_handshake->setMessageSeq(epoch_send_seq_map_[0]);
        resp_msg->setMsg(std::move(resp_handshake));
        auto resp_size = resp_msg->size();

        std::unique_ptr<uint8_t[]> data = std::unique_ptr<uint8_t[]>(new uint8_t[resp_size]);
        int32_t consumed = resp_msg->encode(data.get(), resp_size);
        if (consumed < 0)
        { // todo:add log
            return false;
        }
        handshake_data_.append((char*)data.get() + DTLS_HEADER_SIZE, resp_size - DTLS_HEADER_SIZE);
        sock->sendTo(std::move(data), resp_size, remote_ep, yield);
        epoch_send_seq_map_[0]++;
        std::cout << "append handshake data" << std::endl;
    }
    epoch_receive_seq_map_[recv_msg->getEpoch()] = recv_msg->getSequenceNo() + 1;
    next_msg_require_handler_ = std::bind(&DtlsSession::requireClientKeyExchange, this);
    next_msg_handler_ = std::bind(&DtlsSession::processClientKeyExchange, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
    return true;
}

std::shared_ptr<DTLSPlaintext> DtlsSession::requireClientKeyExchange()
{
    std::shared_ptr<DTLSPlaintext> dtls_msg;
    std::map<uint64_t, std::shared_ptr<DTLSPlaintext>>::iterator it;
    for (it = unhandled_msgs_.begin(); it != unhandled_msgs_.end(); it++)
    {
        if (it->second->getType() != handshake)
        {
            continue;
        }

        HandShake *handshake = (HandShake *)it->second->msg.get();
        if (handshake->getType() != client_key_exchange)
        {
            continue;
        }

        dtls_msg = it->second;
        unhandled_msgs_.erase(it);
        return dtls_msg;
    }
    return nullptr;
}

bool DtlsSession::processClientKeyExchange(std::shared_ptr<DTLSPlaintext> msg, UdpSocket *sock, const boost::asio::ip::udp::endpoint &remote_ep, boost::asio::yield_context &yield)
{
    HandShake *recv_handshake_msg = (HandShake *)msg->msg.get();
    ClientKeyExchange *client_key_exchange_msg = (ClientKeyExchange *)recv_handshake_msg->msg.get();
    std::string decoded_data;
    decoded_data.resize(1024);
    int n = decryptRSA(client_key_exchange_msg->enc_pre_master_secret.pre_master_secret, decoded_data);
    if (n < 0)
    {
        return false;
    }

    std::string pre_master_secret_raw;
    pre_master_secret_raw.assign(decoded_data.data(), n);
    int consumed = pre_master_secret_.decode((uint8_t *)pre_master_secret_raw.data(), n);
    if (consumed < 0)
    {
        return -1;
    }
    HandShake *recv_client_hello_msg = (HandShake *)client_hello_->msg.get();
    ClientHello *client_hello = (ClientHello *)recv_client_hello_msg->msg.get();
    if (client_hello->client_version != pre_master_secret_.client_version)
    {
        return false;
    }

    // 生成master secret
    HandShake *send_server_hello_msg = (HandShake *)server_hello_->msg.get();
    ServerHello *server_hello = (ServerHello *)send_server_hello_msg->msg.get();
    std::string master_key_seed;
    master_key_seed.append((char *)client_hello->random.random_raw, 32);
    master_key_seed.append((char *)server_hello->random.random_raw, 32);
    master_secret_ = PRF(pre_master_secret_raw, "master secret", master_key_seed, 48);
    memcpy(security_params_.master_secret, master_secret_.data(), 48);
    security_params_.client_random.assign((char*)client_hello->random.random_raw, 32);
    security_params_.server_random.assign((char*)server_hello->random.random_raw, 32);

    ciper_suite_->init(master_secret_, security_params_.client_random, security_params_.server_random, false);
    // 生成 srtp key material
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
    // 生成srtp key
    std::string key_material_seed;
    key_material_seed.append((char *)server_hello->random.random_raw, 32);
    key_material_seed.append((char *)client_hello->random.random_raw, 32);
    
    size_t srtp_cipher_key_length = 16;  // 128/8
    size_t srtp_cipher_salt_length = 14; // 112/8
    // total:(16+14)*2
    size_t total_key_material_need = (srtp_cipher_key_length + srtp_cipher_salt_length) * 2;
    std::string srtp_key_block = PRF(master_secret_, "EXTRACTOR-dtls_srtp", key_material_seed, total_key_material_need);
    size_t offset = 0;
    std::string client_master_key(srtp_key_block.data(), srtp_cipher_key_length);
    offset += srtp_cipher_key_length;
    std::string server_master_key((char *)(srtp_key_block.data() + offset), srtp_cipher_key_length);
    offset += srtp_cipher_key_length;
    std::string client_master_salt((char *)(srtp_key_block.data() + offset), srtp_cipher_salt_length);
    offset += srtp_cipher_salt_length;
    std::string server_master_salt((char *)(srtp_key_block.data() + offset), srtp_cipher_salt_length);

    recv_key_ = client_master_key + client_master_salt;
    send_key_ = server_master_key + server_master_salt;

    epoch_receive_seq_map_[msg->getEpoch()] = msg->getSequenceNo() + 1;
    next_msg_require_handler_ = std::bind(&DtlsSession::requireChangeCipherSpec, this);
    next_msg_handler_ = std::bind(&DtlsSession::processChangeCipherSpec, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
    return true;
}

std::shared_ptr<DTLSPlaintext> DtlsSession::requireChangeCipherSpec()
{
    std::shared_ptr<DTLSPlaintext> dtls_msg;
    std::map<uint64_t, std::shared_ptr<DTLSPlaintext>>::iterator it;
    for (it = unhandled_msgs_.begin(); it != unhandled_msgs_.end(); it++)
    {
        if (it->second->getType() != change_cipher_spec)
        {
            continue;
        }

        dtls_msg = it->second;
        unhandled_msgs_.erase(it);
        return dtls_msg;
    }
    
    return nullptr;
}

bool DtlsSession::processChangeCipherSpec(std::shared_ptr<DTLSPlaintext> msg, UdpSocket *sock, const boost::asio::ip::udp::endpoint &remote_ep, boost::asio::yield_context & yield)
{
    ciper_state_changed_ = true;
    epoch_receive_seq_map_[msg->getEpoch()] = msg->getSequenceNo() + 1;
    next_msg_require_handler_ = std::bind(&DtlsSession::requireDone, this);
    next_msg_handler_ = std::bind(&DtlsSession::processDone, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
    return true;
}

bool DtlsSession::processHandShakeFinished(std::shared_ptr<DTLSCiperText> dtls_msg, UdpSocket *sock, const boost::asio::ip::udp::endpoint &remote_ep, boost::asio::yield_context & yield)
{
    // 发送change ciper spec
    HandShake handshake_msg;
    std::string & content = dtls_msg->getBlockCiper().getBlockCipered().content;
    std::cout << "content.size():" << content.size() << std::endl;
    handshake_msg.decode((uint8_t*)content.data(), content.size());

    DtlsFinished *dtls_finished = (DtlsFinished*)handshake_msg.msg.get();

    std::string verify_data = PRF(master_secret_, "client finished", Utils::sha256(handshake_data_), 12);
    if (verify_data != dtls_finished->getVerifyData()) {
        std::cout << "verify data not equal" << std::endl;
        return false;
    } 

    { // send change ciper spec
        std::shared_ptr<DTLSPlaintext> resp_msg = std::make_shared<DTLSPlaintext>();
        resp_msg->setType(change_cipher_spec);
        resp_msg->setDtlsProtocolVersion(DtlsProtocolVersion(DTLS_MAJOR_VERSION1, DTLS_MINOR_VERSION2));
        resp_msg->setEpoch(0);
        resp_msg->setSequenceNo(epoch_send_seq_map_[0]);
        
        std::unique_ptr<ChangeCipherSpec> resp_change_cipher_spec = std::unique_ptr<ChangeCipherSpec>(new ChangeCipherSpec);
        resp_msg->setMsg(std::move(resp_change_cipher_spec));
        auto resp_size = resp_msg->size();

        std::unique_ptr<uint8_t[]> data = std::unique_ptr<uint8_t[]>(new uint8_t[resp_size]);
        int32_t consumed = resp_msg->encode(data.get(), resp_size);
        if (consumed < 0)
        { // todo:add log
            return false;
        }
        epoch_send_seq_map_[0]++;
        sock->sendTo(std::move(data), resp_size, remote_ep, yield);
    }

    {//发送server finished 
        DTLSPlaintext resp_msg;
        resp_msg.setType(handshake);
        resp_msg.setDtlsProtocolVersion(DtlsProtocolVersion(DTLS_MAJOR_VERSION1, DTLS_MINOR_VERSION2));
        resp_msg.setEpoch(1);
        resp_msg.setSequenceNo(epoch_send_seq_map_[1]);

        std::unique_ptr<HandShake> resp_handshake = std::unique_ptr<HandShake>(new HandShake);
        resp_handshake->setType(finished);
        auto *s = new DtlsFinished;
        std::unique_ptr<HandShakeMsg> resp_finished = std::unique_ptr<HandShakeMsg>(s);
        resp_handshake->setMsg(std::move(resp_finished));
        resp_handshake->setMessageSeq(epoch_send_seq_map_[1]);
        resp_msg.setMsg(std::move(resp_handshake));
        auto resp_size = resp_msg.size();

        DTLSCiperText ciper_resp_msg;
        std::string out;
        int32_t consumed = ciper_resp_msg.encode(resp_msg, ciper_suite_.get(), out);
        if (consumed < 0)
        {
            return false;
        }

        epoch_send_seq_map_[1]++;
    }
    // 已经收到finished消息了，不需要再处理后续消息
    next_msg_handler_ = std::bind(&DtlsSession::processDone, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
    return true;
}

std::shared_ptr<DTLSPlaintext> DtlsSession::requireDone()
{
    return nullptr;
}

bool DtlsSession::processDone(std::shared_ptr<DTLSPlaintext> msg, UdpSocket *sock, const boost::asio::ip::udp::endpoint &remote_ep, boost::asio::yield_context & yield)
{
    return true;
}

