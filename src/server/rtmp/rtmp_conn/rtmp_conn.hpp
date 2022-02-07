#pragma once
#include <memory>
#include <unordered_map>
#include <boost/asio/ip/tcp.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>

#include "base/thread/thread_worker.hpp"
#include "base/network/tcp_socket.hpp"

namespace mms {
class RtmpSession;
class RtmpConn : public TcpSocket {
    friend class RtmpSession;
public:
    RtmpConn(TcpSocketHandler *handler, boost::asio::ip::tcp::socket *sock, ThreadWorker *worker, boost::asio::yield_context y):TcpSocket(handler, sock, worker, y) {
    }
    std::shared_ptr<RtmpSession> createSession();
    std::shared_ptr<RtmpSession> getSession();
private:
    std::shared_ptr<RtmpSession> session_ = nullptr;
    uint64_t recv_size_;
    uint64_t send_size_;
};
};