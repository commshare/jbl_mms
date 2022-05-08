#pragma once
#include "base/network/tls_socket.hpp"

#include "server/udp/udp_server.hpp"
#include "server/tcp/tcp_server.hpp"
#include "base/network/udp_socket.hpp"
#include "base/thread/thread_pool.hpp"

#include "protocol/stun_define.hpp"
#include "protocol/stun_binding_error_response_msg.hpp"
#include "protocol/stun_binding_response_msg.hpp"

namespace mms {
#define STUN_DEFAULT_PORT 3478
class StunServer : public UdpServer, public UdpSocketHandler {
public:
    StunServer(ThreadWorker *worker) : UdpServer(worker){

    };

    virtual ~StunServer() {

    }
public:
    bool start(uint32_t port = STUN_DEFAULT_PORT) {
        bool ret = startListen(port);
        return ret;
    }
private:
    void onUdpSocketRecv(UdpSocket *sock, std::unique_ptr<uint8_t[]> data, size_t len, boost::asio::ip::udp::endpoint &remote_ep);
    void processBindMsg(StunMsg & msg, UdpSocket *sock, const boost::asio::ip::udp::endpoint & remote_ep, boost::asio::yield_context & yield);
};
};