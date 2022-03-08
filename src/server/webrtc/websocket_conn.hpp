#pragma once
#include "base/thread/thread_worker.hpp"
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

namespace mms {
class WebRtcSession;
class WebSocketConn {
public:
    WebSocketConn(ThreadWorker *worker, websocketpp::server<websocketpp::config::asio>::connection_ptr c);
    ~WebSocketConn();

    std::shared_ptr<WebRtcSession> createSession();
    std::shared_ptr<WebRtcSession> getSession();
    void destroySession();
private:
    websocketpp::server<websocketpp::config::asio>::connection_ptr ws_conn_;
    std::shared_ptr<WebRtcSession> session_;
    ThreadWorker *worker_;
};
};