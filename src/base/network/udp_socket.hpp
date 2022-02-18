#pragma once
#include <boost/atomic.hpp>
#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp>
#include "base/thread/thread_pool.hpp"

namespace mms {
class UdpSocket;
class UdpSocketHandler {
public:
    virtual void onUdpSocketRecv(UdpSocket *sock, uint8_t *data, size_t len, boost::asio::ip::udp::endpoint remote_endpoint) = 0;
};

class UdpSocket {
public:
    UdpSocket(UdpSocketHandler *handler, boost::asio::ip::udp::socket *sock, ThreadWorker *worker) : handler_(handler), socket_(sock), worker_(worker) {

    }

    virtual ~UdpSocket() {

    }

    bool sendTo(const uint8_t *data, size_t len, const boost::asio::ip::udp::endpoint & remote_pt, boost::asio::yield_context & yield) {
        boost::system::error_code ec;
        size_t size = socket_->async_send_to(boost::asio::buffer(data, len), remote_pt, yield[ec]);
        if (ec) {
            return false;
        }
        send_bytes_ += len;
        return true;
    }

    bool recvFrom(uint8_t *data, size_t len, boost::asio::yield_context & yield) {
        boost::system::error_code ec;
        boost::asio::ip::udp::endpoint remote_endpoint;
        size_t size = socket_->async_receive_from(boost::asio::buffer(data, len), remote_endpoint, yield[ec]);
        if (!ec) {
            recv_bytes_ += size;
            handler_->onUdpSocketRecv(this, data, size, remote_endpoint);
            return true;
        }
        return false;
    }

    uint64_t getRecvCount();
    uint64_t getSendCount();
    inline ThreadWorker *getWorker() {
        return worker_;
    }
protected:
    uint64_t send_bytes_ = 0;
    uint64_t recv_bytes_ = 0;
    UdpSocketHandler *handler_;
    std::unique_ptr<boost::asio::ip::udp::socket> socket_ = nullptr;
    ThreadWorker *worker_ = nullptr;
};
};