#include "webrtc_server.hpp"
using namespace mms;

bool WebRtcServer::start() {
    bool ret = RoomServer::start();
    if (!ret) {
        return false;
    }

    ret = UdpServer::startListen(2022);
    if (!ret) {
        return false;
    }   
    return true;
}

void WebRtcServer::onUdpSocketRecv(UdpSocket *sock, std::unique_ptr<uint8_t[]> data, size_t len, boost::asio::ip::udp::endpoint &remote_ep) {
    auto worker = thread_pool_inst::get_mutable_instance().getWorker(-1);
    std::cout << "recv len:" << len << std::endl;
    boost::asio::spawn(worker->getIOContext(), [this, sock, recv_data = std::move(data), len, remote_ep](boost::asio::yield_context yield) {
    });
}