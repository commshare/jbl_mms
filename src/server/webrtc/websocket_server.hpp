#pragma once
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include "base/thread/thread_worker.hpp"

namespace mms {
#define DEFAULT_ROOM_SERVER_PORT 8282
class WebsocketServer : public websocketpp::server<websocketpp::config::asio> {
public:
    WebsocketServer() = default;
    virtual ~WebsocketServer();
public:
    bool start(uint16_t port = DEFAULT_ROOM_SERVER_PORT);
    void stop();
protected:
    virtual void onWebsocketOpen(websocketpp::connection_hdl hdl) = 0;
    virtual void onWebsocketClose(websocketpp::connection_hdl hdl) = 0;
    void onMessage(websocketpp::server<websocketpp::config::asio>* server, websocketpp::connection_hdl hdl, message_ptr msg);
private:
    std::shared_ptr<std::thread> work_thread_;
};
};