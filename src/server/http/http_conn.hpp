#pragma once
#include <memory>
#include <unordered_map>
#include <boost/asio/ip/tcp.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>

#include "base/thread/thread_worker.hpp"
#include "base/network/tcp_socket.hpp"

namespace mms {
class HttpConn : public TcpSocket {
public:
    HttpConn(boost::asio::ip::tcp::socket *sock, ThreadWorker *worker, boost::asio::yield_context y):TcpSocket(sock, worker, y) {
    }

private:
    uint64_t recv_size_ = 0;
    uint64_t send_size_ = 0;
};
};