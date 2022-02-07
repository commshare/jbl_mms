#pragma once
#include <boost/atomic.hpp>
#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp>
#include "base/thread/thread_pool.hpp"

namespace mms {
class TcpSocket;
class TcpSocketHandler {
public:
    virtual ~TcpSocketHandler() {}
    virtual void onTcpSocketOpen(TcpSocket *sock) = 0;
    virtual void onTcpSocketClose(TcpSocket *sock) = 0;
};

class TcpSocket {
public:
    TcpSocket(TcpSocketHandler *handler, boost::asio::ip::tcp::socket *sock, ThreadWorker *worker, boost::asio::yield_context y);

    virtual ~TcpSocket();

    bool send(const uint8_t *data, size_t len);
    bool recv(uint8_t *data, size_t len);
    int32_t recvSome(uint8_t *data, size_t len);
    void open();
    void close();

    uint64_t getRecvCount();
    uint64_t getSendCount();
    inline ThreadWorker *getWorker() {
        return worker_;
    }
private:
    boost::asio::ip::tcp::socket *socket_ = nullptr;
    ThreadWorker *worker_ = nullptr;
    boost::asio::yield_context yield_;
    boost::atomic_uint64_t in_bytes_;
    boost::atomic_uint64_t out_bytes_;
    TcpSocketHandler *handler_;
};
};