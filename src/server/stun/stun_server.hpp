#pragma once
#include "base/network/tls_socket.hpp"

#include "server/udp/udp_server.hpp"
#include "server/tcp/tcp_server.hpp"

#include "protocol/stun_define.hpp"
#include "protocol/stun_binding_error_response_msg.hpp"

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
        StunMsg stun_msg;
        stun_msg.decode(data, len);
        switch(stun_msg.type()) {
            case STUN_BINDING_REQUEST : {
                processBindMsg(stun_msg, sock, remote_endpoint);
                break;
            }
        }
    }
private:
    void processBindMsg(StunMsg & msg, UdpSocket *sock, boost::asio::ip::udp::endpoint & remote_ep) {
        if (msg.attrs.size() <= 0) {// no message integrity
            StunBindingErrorResponseMsg resp(401, "");
            auto s = resp.size();
            boost::array<uint8_t, s> data;
            sock->sendTo(data.data(), s, remote_pt, yield);
        }
    }
};
};