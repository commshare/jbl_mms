#pragma once
#include "base/network/tls_socket.hpp"

#include "server/udp/udp_server.hpp"
#include "server/tcp/tcp_server.hpp"

#include "protocol/stun_define.hpp"

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
        int32_t ret = startListen(port);
        if (0 != ret) {
            return false;
        }
        return true;
    }
private:
    void onUdpSocketRecv(UdpSocket *sock, uint8_t *data, size_t len, boost::asio::ip::udp::endpoint remote_endpoint) {
        std::cout << "recv udp len:" << len << std::endl;
        StunMsg stun_msg;
        stun_msg.decode(data, len);
    }
};
};