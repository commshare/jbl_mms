#pragma once
#include <memory>
#include <unordered_map>
#include <boost/asio/ip/tcp.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>
#include <functional>

#include "base/thread/thread_worker.hpp"
#include "base/network/tcp_socket.hpp"


namespace mms {
#define HTTP_MAX_BUF (1024*1024)
class HttpSession;
class HttpConn : public TcpSocket {
    friend class HttpSession;
public:
    HttpConn(TcpSocketHandler *handler, boost::asio::ip::tcp::socket *sock, ThreadWorker *worker);
    void cycleRecv(const std::function<int32_t(const char *buf, size_t len, boost::asio::yield_context & yield)> & recv_handler, boost::asio::yield_context & yield);

    std::shared_ptr<HttpSession> createSession();
    std::shared_ptr<HttpSession> getSession();
    void destroySession();
private:
    std::string buf_;
    size_t buf_size_ = 0;
    size_t buf_pos_ = 0;
    std::shared_ptr<HttpSession> session_;
};
};