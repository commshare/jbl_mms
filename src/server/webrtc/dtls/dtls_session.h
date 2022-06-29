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

#include "server/dtls/extension/dtls_use_srtp.h"

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
    void onHandshakeDone(const std::function<void(SRTPProtectionProfile profile, const std::string & srtp_recv_key, const std::string & srtp_send_key)> & cb)
    {
        handshake_done_cb_ = cb;
    }
private:
    bool processClientHello(std::shared_ptr<DTLSPlaintext> msg, UdpSocket *sock, const boost::asio::ip::udp::endpoint &remote_ep, boost::asio::yield_context & yield);
    bool processClientHelloWithCookie(std::shared_ptr<DTLSPlaintext> msg, UdpSocket *sock, const boost::asio::ip::udp::endpoint &remote_ep, boost::asio::yield_context & yield);
    bool processClientKeyExchange(std::shared_ptr<DTLSPlaintext> msg, UdpSocket *sock, const boost::asio::ip::udp::endpoint &remote_ep, boost::asio::yield_context & yield);
    bool processChangeCipherSpec(std::shared_ptr<DTLSPlaintext> msg, UdpSocket *sock, const boost::asio::ip::udp::endpoint &remote_ep, boost::asio::yield_context & yield);
    bool processHandShakeFinished(std::shared_ptr<DTLSCiperText> msg, UdpSocket *sock, const boost::asio::ip::udp::endpoint &remote_ep, boost::asio::yield_context & yield);
    
    bool processDone(std::shared_ptr<DTLSPlaintext> msg, UdpSocket *sock, const boost::asio::ip::udp::endpoint &remote_ep, boost::asio::yield_context & yield);
    int32_t decryptRSA(const std::string & enc_data, std::string & dec_data);
private:
    std::shared_ptr<DTLSPlaintext> requireClientHello();
    std::shared_ptr<DTLSPlaintext> requireClientHelloWithCookie();
    std::shared_ptr<DTLSPlaintext> requireClientKeyExchange();
    std::shared_ptr<DTLSPlaintext> requireChangeCipherSpec();
    std::shared_ptr<DTLSPlaintext> requireDone();
private:
    std::shared_ptr<DTLSPlaintext> client_hello_;
    std::shared_ptr<DTLSPlaintext> server_hello_;
    PreMasterSecret pre_master_secret_;
    std::string master_secret_;
    uint32_t send_message_seq_ = 0;

    SecurityParameters security_params_;
    std::string srtp_recv_key_;
    std::string srtp_send_key_;

    std::unique_ptr<DtlsCiperSuite> ciper_suite_;
    std::shared_ptr<DtlsCert> dtls_cert_;

    std::map<uint64_t, std::shared_ptr<DTLSPlaintext>> unhandled_msgs_;
    std::list<std::shared_ptr<DTLSPlaintext>> handshake_msgs_;
    std::queue<std::shared_ptr<DTLSPlaintext>> sended_msgs_;
    std::unordered_map<HandshakeType, bool> recv_handshake_map_;
    std::string handshake_data_;
    std::shared_ptr<DTLSCiperText> recv_finished_msg_;
    //    DTLS implementations maintain (at least notionally) a
    //    next_receive_seq counter.  This counter is initially set to zero.
    //    When a message is received, if its sequence number matches
    //    next_receive_seq, next_receive_seq is incremented and the message is
    //    processed.  If the sequence number is less than next_receive_seq, the
    //    message MUST be discarded.  If the sequence number is greater than
    //    next_receive_seq, the implementation SHOULD queue the message but MAY
    //    discard it.  (This is a simple space/bandwidth tradeoff).
    std::unordered_map<uint16_t, uint32_t> epoch_receive_seq_map_;
    std::unordered_map<uint16_t, uint32_t> epoch_send_seq_map_;

    ThreadWorker::Event *retrans_event_ = nullptr;
    bool ciper_state_changed_ = false;
    std::function<std::shared_ptr<DTLSPlaintext>()> next_msg_require_handler_;
    std::function<bool(std::shared_ptr<DTLSPlaintext> msg, UdpSocket *sock, const boost::asio::ip::udp::endpoint &remote_ep, boost::asio::yield_context & yield)> next_msg_handler_;

    std::function<void(SRTPProtectionProfile profile, const std::string & srtp_recv_key, const std::string & srtp_send_key)> handshake_done_cb_;
};
};