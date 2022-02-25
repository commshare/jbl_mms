#pragma once
#include <memory>

namespace mms {
class UdpSocket;
class UdpSocketHandler {
public:
    virtual void onUdpSocketRecv(UdpSocket *sock, std::unique_ptr<uint8_t[]> data, size_t len, boost::asio::ip::udp::endpoint &remote_ep) = 0;
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