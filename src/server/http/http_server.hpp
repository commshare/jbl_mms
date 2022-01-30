#pragma once
#include <memory>

#include "server/tcp/tcp_server.hpp"
#include "server/http/http_conn.hpp"
#include "base/thread/thread_pool.hpp"

namespace mms {
class HttpServer : public TcpServer<HttpConn>, ServerConnHandler<HttpConn> {
public:
    HttpServer(ThreadWorker *w):TcpServer(w) {
        setConnHandler(this);
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
    void onConnOpen(HttpConn *socket) override;
    void onConnClosed(HttpConn *socket) override;
};
};