#include <iostream>
#include <boost/asio/experimental/as_tuple.hpp>

#include "tcp_socket.hpp"

namespace mms {

TcpSocket::TcpSocket(TcpSocketHandler *handler, boost::asio::ip::tcp::socket *sock, ThreadWorker *worker) : 
    socket_(sock), worker_(worker), in_bytes_(0), out_bytes_(0) {
    handler_ = handler;
}

TcpSocket::~TcpSocket() {
    if (socket_) {
        delete socket_;
        socket_ = nullptr;
        std::cout << "******************* destroy TcpSocket ******************" << std::endl;
    }
}

void TcpSocket::open() {
    handler_->onTcpSocketOpen(this);
}

uint64_t TcpSocket::getRecvCount() {
    return in_bytes_;
}

uint64_t TcpSocket::getSendCount() {
    return out_bytes_;
}

boost::asio::awaitable<bool> TcpSocket::send(const uint8_t *data, size_t len) {
    size_t pos = 0;
    while (pos < len) {
        auto [ec, s] = co_await socket_->async_send(boost::asio::buffer(data + pos, len - pos), 0, boost::asio::experimental::as_tuple(boost::asio::use_awaitable));
        if(ec) {
            co_return false;
        }
        pos += s;
    }
    out_bytes_ += len;
    co_return true;
}

boost::asio::awaitable<bool> TcpSocket::recv(uint8_t *data, size_t len) {
    size_t pos = 0;
    while (pos < len) {
        std::cout << "start async recv..." << std::endl;
        auto [ec, s] = co_await socket_->async_receive(boost::asio::buffer(data + pos, len - pos), boost::asio::experimental::as_tuple(boost::asio::use_awaitable));
        if (ec) {
            co_return false;
        }
        pos += s;
    }
    
    in_bytes_ += len;
    co_return true;
}

boost::asio::awaitable<int32_t> TcpSocket::recvSome(uint8_t *data, size_t len) {
    auto [ec, s] = co_await socket_->async_read_some(boost::asio::buffer(data, len), boost::asio::experimental::as_tuple(boost::asio::use_awaitable));
    if (ec) {
        co_return -1;
    }
    in_bytes_ += s;
    co_return s;
}

void TcpSocket::close() {
    if (closed_) {
        return;
    }
    closed_ = true;

    if (handler_) {
        handler_->onTcpSocketClose(this);
    }
    socket_->close();
    delete this;
}


};