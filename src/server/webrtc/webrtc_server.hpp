#pragma once
#include <unordered_map>
#include <memory>
#include <iostream>
#include "webrtc_session.hpp"
#include "server/udp/udp_server.hpp"
#include "websocket_server.hpp"

#include "server/stun/protocol/stun_msg.h"
namespace mms {
class WebSocketConn;

enum UDP_MSG_TYPE {
    UDP_MSG_UNKNOWN = 0,
    UDP_MSG_STUN    = 1,
    UDP_MSG_ZRTP    = 2,
    UDP_MSG_DTLS    = 3,
    UDP_MSG_TURN    = 4,
    UDP_MSG_RTP     = 5,
};

class WebRtcServer : public UdpServer, public WebsocketServer {
public:
    WebRtcServer(ThreadWorker *worker) : UdpServer(worker) {
        worker_ = worker;
    }

    bool start();
private:
    void onUdpSocketRecv(UdpSocket *sock, std::unique_ptr<uint8_t[]> data, size_t len, boost::asio::ip::udp::endpoint &remote_ep) override;
    bool processStunPacket(StunMsg &stun_msg, uint8_t *data, size_t len, UdpSocket *sock, const boost::asio::ip::udp::endpoint &remote_ep, boost::asio::yield_context & yield);
    bool processDtlsPacket(uint8_t *data, size_t len, UdpSocket *sock, const boost::asio::ip::udp::endpoint &remote_ep, boost::asio::yield_context & yield);
private:
    virtual void onWebsocketOpen(websocketpp::connection_hdl hdl);
    virtual void onWebsocketClose(websocketpp::connection_hdl hdl);
private:
    UDP_MSG_TYPE detectMsgType(uint8_t * data, size_t len);
private:
    ThreadWorker *worker_;
    std::mutex mtx_;
    std::unordered_map<websocketpp::server<websocketpp::config::asio>::connection_ptr, std::shared_ptr<WebSocketConn>> conn_map_;
    std::mutex session_map_mtx_;
    std::unordered_map<boost::asio::ip::udp::endpoint, std::shared_ptr<WebRtcSession>> endpoint_session_map_;
    std::unordered_map<std::string, std::shared_ptr<WebRtcSession>> ufrag_session_map_;
};
};