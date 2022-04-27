#include "websocket_conn.hpp"
#include "webrtc_session.hpp"

using namespace mms;

WebSocketConn::WebSocketConn(ThreadWorker *worker, websocketpp::server<websocketpp::config::asio>::connection_ptr c) : ws_conn_(c), worker_(worker) {

}

WebSocketConn::~WebSocketConn() {

}

std::shared_ptr<WebRtcSession> WebSocketConn::createSession() {
    session_ = std::make_shared<WebRtcSession>(worker_, this);
    return session_;
}

std::shared_ptr<WebRtcSession> WebSocketConn::getSession() {
    return session_;
}

void WebSocketConn::destroySession() {
    session_.reset();
}