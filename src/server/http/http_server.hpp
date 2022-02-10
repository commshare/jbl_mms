#pragma once
#include <memory>

#include "server/tcp/tcp_server.hpp"
#include "server/http/http_conn.hpp"
#include "base/thread/thread_pool.hpp"
#include "server/http/http_protocol/http_define.hpp"
#include "http_flv.hpp"
#include "http_session.hpp"
#include "http_handler.hpp"

namespace mms {
class HttpServer : public TcpServer<HttpConn> {
public:
    HttpServer(ThreadWorker *w):TcpServer(w) {
    }
    
    bool start() {
        if (0 == startListen(80)) {
            return true;
        } 
        return false;
    }

    void stop() {
        stopListen();
    }
private:
    void onTcpSocketOpen(TcpSocket *socket) override;
    void onTcpSocketClose(TcpSocket *socket) override;
};
};