#include "http_conn.hpp"
using namespace mms;

HttpConn::HttpConn(TcpSocketHandler *handler, boost::asio::ip::tcp::socket *sock, ThreadWorker *worker):TcpSocket(handler, sock, worker) {
    buf_.resize(HTTP_MAX_BUF);
}

void HttpConn::cycleRecv(const std::function<int32_t(const char *, size_t, boost::asio::yield_context &)> & recv_handler, boost::asio::yield_context & yield) {
    while(1) {
        auto recv_size = recvSome((uint8_t*)buf_.data() + buf_size_, HTTP_MAX_BUF - buf_size_, yield);
        if (recv_size < 0) {
            break;
        }

        buf_size_ += recv_size;
        int32_t consumed = recv_handler((const char*)buf_.data(), buf_size_, yield);
        if (consumed < 0) {
            close();
            return;
        }

        if (consumed > 0) {
            memmove((void*)buf_.data(), (void*)(buf_.data() + consumed), buf_size_ - consumed);
            buf_size_ -= consumed;
        }
    }
}