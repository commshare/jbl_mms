#pragma once
#include <memory>

#include "base/thread/thread_worker.hpp"
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include "core/session.hpp"

namespace mms {
class WebsocketServer;
class WebSocketConn;
class WebRtcSession  : public Session, public std::enable_shared_from_this<WebRtcSession>{
public:
    WebRtcSession(ThreadWorker *worker, WebSocketConn *conn);
    virtual ~WebRtcSession();

    void service();
    void close() override;
    ThreadWorker *getWorker() {
        return worker_;
    }
    void onMessage(websocketpp::server<websocketpp::config::asio>* server, websocketpp::connection_hdl hdl, websocketpp::server<websocketpp::config::asio>::message_ptr msg);
private:
    void processOfferMsg(const std::string & sdp);
private:
    ThreadWorker *worker_;
    WebSocketConn *ws_conn_;
};

};