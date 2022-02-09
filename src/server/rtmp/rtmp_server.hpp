#pragma once
#include <memory>

#include "server/tcp/tcp_server.hpp"
#include "server/rtmp/rtmp_conn/rtmp_conn.hpp"
#include "base/thread/thread_pool.hpp"

namespace mms {
class RtmpServer : public TcpServer<RtmpConn> {
public:
    RtmpServer(ThreadWorker *w):TcpServer(w) {
    }
    
    bool start() {
        if (0 == startListen(1935)) {
            return true;
        }
        return false;
    }

    void stop() {
        stopListen();
    }
private:
    void onTcpSocketOpen(TcpSocket *socket, boost::asio::yield_context & yield) override;
    void onTcpSocketClose(TcpSocket *socket, boost::asio::yield_context & yield) override;
};
};