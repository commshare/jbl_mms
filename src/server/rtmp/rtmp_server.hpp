#pragma once
#include "server/tcp/tcp_server.hpp"
#include "rtmp_context.hpp"
#include "base/thread/thread_pool.hpp"

namespace mms {
class RtmpServer : public TcpServer<RtmpContext> {
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
};
};