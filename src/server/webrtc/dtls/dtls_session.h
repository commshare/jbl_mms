#pragma once
#include <optional>
#include <queue>
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
    int32_t decryptRSA(const std::string & enc_data, std::string & dec_data);
    bool calcMasterSecret();
private:
    int32_t expectClientHello(uint8_t *data, size_t len, UdpSocket *sock, const boost::asio::ip::udp::endpoint &remote_ep, boost::asio::yield_context & yield);
    int32_t expectClientKeyExchange(uint8_t *data, size_t len, UdpSocket *sock, const boost::asio::ip::udp::endpoint &remote_ep, boost::asio::yield_context & yield);
    int32_t expectHandShakeFinished(uint8_t *data, size_t len, UdpSocket *sock, const boost::asio::ip::udp::endpoint &remote_ep, boost::asio::yield_context & yield);
private:
    enum DtlsState {
        DtlsStateInit = 0,
        DtlsStateRecvClientHello = 1,
        DtlsStateSendServerCertificate = 2,
        DtlsStateSendServerDone = 3,
        DtlsStateSendServerHello = 4,
        DtlsStateKeyExchangeDone = 5,
        DtlsStateChangeCipher    = 6,
        DtlsStateHandShakeFinished = 7,
    };

    DtlsState state_ = DtlsStateInit;
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
    std::shared_ptr<DTLSPlaintext> last_msg_;
    std::queue<std::shared_ptr<DTLSPlaintext>> sended_msgs_;
    uint32_t last_message_req_ = 0;
    ThreadWorker::Event *retrans_event_ = nullptr;
    std::function<int32_t(uint8_t *data, size_t len, UdpSocket *sock, const boost::asio::ip::udp::endpoint &remote_ep, boost::asio::yield_context & yield)> next_handler_;
};
};