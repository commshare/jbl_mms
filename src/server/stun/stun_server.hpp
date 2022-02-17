#pragma once
#include "base/network/tls_socket.hpp"

#include "server/udp/udp_server.hpp"
#include "server/tcp/tcp_server.hpp"

#include "protocol/stun_define.hpp"

namespace mms {
#define STUN_DEFAULT_PORT 3478
class StunServer : public UdpServer, public TcpServer<TlsSocket> {
public:
    StunServer(ThreadWorker *worker):UdpServer(worker), TcpServer<TlsSocket>(worker) {

    };

    virtual ~StunServer() {

    }
public:
    int32_t startListen(uint32_t port = STUN_DEFAULT_PORT) {
        UdpServer::onRecvPkt(std::bind(&StunServer::onRecvPkt, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        // stun 服务器包含两部分，tcp 交换秘钥，udp 绑定消息处理
        int32_t ret = UdpServer::startListen(port);
        if (0 != ret) {
            return -1;
        }

        // 启动TCP 服务

        ret = TcpServer::startListen(port);
        if (0 != ret) {
            return -2;
        }
        return 0;
    }
private:
    int32_t onRecvPkt(boost::asio::ip::udp::socket udp_sock, const uint8_t *data, size_t len) {
        
    }
private:
    void onTcpSocketOpen(TcpSocket *socket) override;
    void onTcpSocketClose(TcpSocket *socket) override;
};
};