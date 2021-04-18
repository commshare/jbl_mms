#pragma once
#include <atomic>
#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp>

#include "../thread/thread_pool.hpp"

namespace mms {
class TcpSocket {
public:
    TcpSocket(boost::asio::ip::tcp::socket *sock, ThreadWorker *worker, boost::asio::yield_context y);

    virtual ~TcpSocket();

    bool send(const char *data, size_t len);
    bool recv(char *data, size_t len);
    int32_t recvSome(char *data, size_t len);
private:
    boost::asio::ip::tcp::socket *socket_;
    ThreadWorker *worker_;
    boost::asio::yield_context yield_;
    std::atomic_uint64_t in_bytes_;
    std::atomic_uint64_t out_bytes_;
};
};