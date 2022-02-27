#pragma once
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include "base/thread/thread_worker.hpp"

namespace mms {
#define DEFAULT_ROOM_SERVER_PORT 9902
class RoomServer : public websocketpp::server<websocketpp::config::asio> {
public:
    RoomServer(ThreadWorker *worker);
    virtual ~RoomServer();
public:
    bool start(uint16_t port = DEFAULT_ROOM_SERVER_PORT);
    void stop();
private:
    void onOpen(websocketpp::connection_hdl hdl);
    void onClose(websocketpp::connection_hdl hdl);
    void onMessage(websocketpp::server<websocketpp::config::asio>* server, websocketpp::connection_hdl hdl, message_ptr msg);
private:
    ThreadWorker *worker_;
};
};