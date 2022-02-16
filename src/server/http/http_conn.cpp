#include "http_conn.hpp"
#include "http_session.hpp"

using namespace mms;

HttpConn::HttpConn(TcpSocketHandler *handler, boost::asio::ip::tcp::socket sock, ThreadWorker *worker):TcpSocket(handler, std::move(sock), worker) {
    buf_.resize(HTTP_MAX_BUF);
}

boost::asio::awaitable<void> HttpConn::cycleRecv(const std::function<int32_t(const char *, size_t)> & recv_handler) {
    while(1) {
        auto recv_size = co_await recvSome((uint8_t*)buf_.data() + buf_size_, HTTP_MAX_BUF - buf_size_);
        if (recv_size < 0) {
            break;
        }

        buf_size_ += recv_size;
        int32_t consumed = recv_handler((const char*)buf_.data(), buf_size_);
        if (consumed < 0) {
            close();
            co_return;
        }

        if (consumed > 0) {
            memmove((void*)buf_.data(), (void*)(buf_.data() + consumed), buf_size_ - consumed);
            buf_size_ -= consumed;
        }
    }
}

std::shared_ptr<HttpSession> HttpConn::createSession() {
    session_ = std::make_shared<HttpSession>(this);
    return session_;
}


std::shared_ptr<HttpSession> HttpConn::getSession() {
    return session_;
}

void HttpConn::destroySession() {
    session_.reset();
}