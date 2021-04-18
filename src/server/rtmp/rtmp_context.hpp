#pragma once
#include <boost/asio/ip/tcp.hpp>
#include <boost/shared_ptr.hpp>

#include "base/thread/thread_worker.hpp"
#include "base/network/tcp_socket.hpp"
namespace mms {
class RtmpContext {
public:
    RtmpContext(TcpSocket *sock) {
        tcp_socket_ = sock;
    }

    void run();
private:
    ThreadWorker *worker_;
    TcpSocket *tcp_socket_;
};
};