#pragma once
#include "server/udp/udp_server.hpp"
#include "room_server.hpp"
namespace mms {
class WebRtcServer : public UdpServer, public RoomServer {
public:
    WebRtcServer(ThreadWorker *worker) : UdpServer(worker), RoomServer(worker) {

    }

    bool start();
private:
    void onUdpSocketRecv(UdpSocket *sock, std::unique_ptr<uint8_t[]> data, size_t len, boost::asio::ip::udp::endpoint &remote_ep);
};
};