#pragma once
<<<<<<< HEAD
#include <boost/atomic.hpp>
#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp>
#include "base/thread/thread_pool.hpp"
=======
#include <memory>
>>>>>>> 9b05d98606ecca65c0db516bb5367ce45d311088

namespace mms {
class UdpSocket;
class UdpSocketHandler {
public:
<<<<<<< HEAD
    virtual void onUdpSocketRecv(UdpSocket *sock, uint8_t *data, size_t len, boost::asio::ip::udp::endpoint remote_endpoint) = 0;
=======
    virtual void onUdpSocketRecv(UdpSocket *sock, std::unique_ptr<uint8_t[]> data, size_t len, boost::asio::ip::udp::endpoint &remote_ep) = 0;
>>>>>>> 9b05d98606ecca65c0db516bb5367ce45d311088
};

class UdpSocket {
public:
    UdpSocket(UdpSocketHandler *handler, std::unique_ptr<boost::asio::ip::udp::socket> sock) : handler_(handler), sock_(std::move(sock)) {

    }

    bool sendTo(std::unique_ptr<uint8_t[]> data, size_t len, const boost::asio::ip::udp::endpoint & remote_ep, boost::asio::yield_context &yield) {
        boost::system::error_code ec;
        auto size = sock_->async_send_to(boost::asio::buffer(data.get(), len), remote_ep, yield[ec]);
        if (ec || size != len) {
            return false;
        }
        return true;
    }

    int32_t recvFrom(uint8_t *data, size_t len, boost::asio::ip::udp::endpoint & remote_ep, boost::asio::yield_context &yield) {
        boost::system::error_code ec;
        auto size = sock_->async_receive_from(boost::asio::buffer(data, len), remote_ep, yield[ec]);
        if (ec) {
            return -1;
        }
        return size;
    }
private:
    UdpSocketHandler *handler_ = nullptr;
    std::unique_ptr<boost::asio::ip::udp::socket> sock_;
};
};