#include <iostream>
#include "tcp_socket.hpp"

namespace mms {

TcpSocket::TcpSocket(boost::asio::ip::tcp::socket *sock, ThreadWorker *worker, boost::asio::yield_context y) : 
    socket_(sock), yield_(y), in_bytes_(0), out_bytes_(0) {

}

TcpSocket::~TcpSocket() {
    
}

uint64_t TcpSocket::getRecvCount() {
    return in_bytes_;
}

uint64_t TcpSocket::getSendCount() {
    return out_bytes_;
}

bool TcpSocket::send(const uint8_t *data, size_t len) {
    boost::system::error_code ec;
    size_t pos = 0;
    while (pos < len) {
        size_t s = socket_->async_send(boost::asio::buffer(data + pos, len - pos), 0, yield_[ec]);
        if(ec) {
            return false;
        }
        pos += s;
    }
    out_bytes_ += len;
    return true;
}

bool TcpSocket::recv(uint8_t *data, size_t len) {
    boost::system::error_code ec;
    size_t pos = 0;
    while (pos < len) {
        size_t s = socket_->async_receive(boost::asio::buffer(data + pos, len - pos), yield_[ec]);
        if (ec) {
            return false;
        }
        pos += s;
    }
    
    in_bytes_ += len;
    return true;
}

int32_t TcpSocket::recvSome(uint8_t *data, size_t len) {
    boost::system::error_code ec;
    auto s = socket_->async_read_some(boost::asio::buffer(data, len), yield_[ec]);
    if (ec) {
        return -1;
    }
    in_bytes_ += s;
    return s;
}

void TcpSocket::close() {
    worker_->dispatch([this]{
        socket_->close();
    });
}


};