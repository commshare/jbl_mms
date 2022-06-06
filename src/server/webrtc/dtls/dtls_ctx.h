#pragma once
#include <optional>

#include <boost/asio/ip/udp.hpp>
#include <boost/asio/spawn.hpp>

#include "base/network/udp_socket.hpp"

#include "server/dtls/dtls_define.h"
namespace mms {
class DtlsCtx {
public:
    DtlsCtx() = default;
    ~DtlsCtx() {

    }

    bool processDtlsPacket(uint8_t *data, size_t len, UdpSocket *sock, const boost::asio::ip::udp::endpoint &remote_ep, boost::asio::yield_context & yield);
public:
    bool init();
private:
    bool processClientHello(DTLSCiphertext & msg, UdpSocket *sock, const boost::asio::ip::udp::endpoint &remote_ep, boost::asio::yield_context & yield);

private:
    std::optional<DTLSCiphertext> client_hello_;
    std::optional<DTLSCiphertext> server_hello_;
};
};