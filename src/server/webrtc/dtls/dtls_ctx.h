#pragma once
#include <optional>

#include <boost/asio/ip/udp.hpp>
#include <boost/asio/spawn.hpp>

#include "base/network/udp_socket.hpp"

#include "server/dtls/dtls_define.h"
#include "dtls_cert.h"
#include "server/dtls/client_key_exchange.h"
namespace mms {
class DtlsCtx {
public:
    DtlsCtx() = default;
    ~DtlsCtx() {

    }

    bool processDtlsPacket(uint8_t *data, size_t len, UdpSocket *sock, const boost::asio::ip::udp::endpoint &remote_ep, boost::asio::yield_context & yield);
public:
    bool init();
    void setDtlsCert(std::shared_ptr<DtlsCert> cert);
private:
    bool processClientHello(DTLSCiphertext & msg, UdpSocket *sock, const boost::asio::ip::udp::endpoint &remote_ep, boost::asio::yield_context & yield);
    bool processClientKeyExchange(DTLSCiphertext & msg, UdpSocket *sock, const boost::asio::ip::udp::endpoint &remote_ep, boost::asio::yield_context & yield);
    int32_t decryptRSA(const std::string & enc_data, std::string & dec_data);
    bool calcMasterSecret();
private:
    std::optional<DTLSCiphertext> client_hello_;
    std::optional<DTLSCiphertext> server_hello_;
    PreMasterSecret pre_master_secret_;
    std::string master_secret_;
    uint32_t message_seq_ = 0;

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
};
};