#pragma once
#include <optional>
#include <queue>
#include <list>
#include <functional>

#include <boost/asio/ip/udp.hpp>
#include <boost/asio/spawn.hpp>

#include "base/network/udp_socket.hpp"

#include "server/dtls/dtls_define.h"
#include "dtls_cert.h"
#include "server/dtls/client_key_exchange.h"
#include "base/thread/thread_worker.hpp"

namespace mms {
class DtlsSession {
public:
    DtlsSession() = default;
    ~DtlsSession() {

    }
public:
    bool init();
    void setDtlsCert(std::shared_ptr<DtlsCert> cert);
    bool processDtlsPacket(uint8_t *data, size_t len, UdpSocket *sock, const boost::asio::ip::udp::endpoint &remote_ep, boost::asio::yield_context & yield);
private:
    bool processClientHello(std::shared_ptr<DTLSPlaintext> msg, UdpSocket *sock, const boost::asio::ip::udp::endpoint &remote_ep, boost::asio::yield_context & yield);
    bool processClientKeyExchange(std::shared_ptr<DTLSPlaintext> msg, UdpSocket *sock, const boost::asio::ip::udp::endpoint &remote_ep, boost::asio::yield_context & yield);
    bool processChangeCipherSpec(std::shared_ptr<DTLSPlaintext> msg, UdpSocket *sock, const boost::asio::ip::udp::endpoint &remote_ep, boost::asio::yield_context & yield);
    bool processHandShakeFinished(std::shared_ptr<DTLSPlaintext> msg, UdpSocket *sock, const boost::asio::ip::udp::endpoint &remote_ep, boost::asio::yield_context & yield);
    int32_t decryptRSA(const std::string & enc_data, std::string & dec_data);
    bool calcMasterSecret();
private:
    std::shared_ptr<DTLSPlaintext> requireClientHello();
    std::shared_ptr<DTLSPlaintext> requireClientKeyExchange();
    std::shared_ptr<DTLSPlaintext> requireChangeCipherSpec();
    std::shared_ptr<DTLSPlaintext> requireHandShakeFinished();
private:
    std::shared_ptr<DTLSPlaintext> client_hello_;
    std::shared_ptr<DTLSPlaintext> server_hello_;
    PreMasterSecret pre_master_secret_;
    std::string master_secret_;
    uint32_t send_message_seq_ = 0;

    SecurityParameters security_params_;
    std::string recv_key_;
    std::string send_key_;

    std::string verify_data_;

    std::string client_write_MAC_key_;
    std::string server_write_MAC_key_;
    std::string client_write_key_;
    std::string server_write_key_;
    std::string client_write_IV_;
    std::string server_write_IV_;

    std::unique_ptr<CiperSuite> ciper_suite_;
    std::shared_ptr<DtlsCert> dtls_cert_;

    std::map<uint64_t, std::shared_ptr<DTLSPlaintext>> unhandled_msgs_;
    std::list<std::shared_ptr<DTLSPlaintext>> handshake_msgs_;
    std::queue<std::shared_ptr<DTLSPlaintext>> sended_msgs_;
    std::string handshake_data_;
    //    DTLS implementations maintain (at least notionally) a
    //    next_receive_seq counter.  This counter is initially set to zero.
    //    When a message is received, if its sequence number matches
    //    next_receive_seq, next_receive_seq is incremented and the message is
    //    processed.  If the sequence number is less than next_receive_seq, the
    //    message MUST be discarded.  If the sequence number is greater than
    //    next_receive_seq, the implementation SHOULD queue the message but MAY
    //    discard it.  (This is a simple space/bandwidth tradeoff).
    uint32_t next_receive_seq_ = 0;
    ThreadWorker::Event *retrans_event_ = nullptr;
    bool ciper_state_changed_ = false;
    uint16_t epoch_ = 0;
    std::shared_ptr<DTLSCiphertext> client_finished_;
    std::function<std::shared_ptr<DTLSPlaintext>()> next_msg_require_handler_;
    std::function<bool(std::shared_ptr<DTLSPlaintext> msg, UdpSocket *sock, const boost::asio::ip::udp::endpoint &remote_ep, boost::asio::yield_context & yield)> next_msg_handler_;
};
};