#pragma once
#include <stdint.h>
#include <string>
#include <iostream>
#include <memory>

#include <boost/array.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/asio/spawn.hpp>

#include "base/thread/thread_pool.hpp"
#include "base/network/udp_socket.hpp"

namespace mms {
#define MAX_UDP_RECV_BUF 2*1024*1024
class UdpServer : public UdpSocketHandler {
public:
    UdpServer(ThreadWorker *worker):worker_(worker) {

    }

    virtual ~UdpServer() {
        if (udp_sock_) {
            delete udp_sock_;
            udp_sock_ = nullptr;
        }
    }

    int32_t startListen(uint16_t port) {
        if (!worker_) {
            return -1;
        }

        boost::asio::spawn(worker_->getIOContext(), [this, port](boost::asio::yield_context yield) {
            running_ = true;
            boost::system::error_code ec;
            auto sock = std::unique_ptr<boost::asio::ip::udp::socket>(new boost::asio::ip::udp::socket(worker_->getIOContext()));
            boost::asio::ip::udp::endpoint local_endpoint(boost::asio::ip::address::from_string("0.0.0.0"), port);
            sock->open(boost::asio::ip::udp::v4());
            sock->set_option(boost::asio::ip::udp::socket::reuse_address(true));
            sock->bind(local_endpoint);
            if (!sock->is_open()) {
                return;
            }

            udp_sock_ = new UdpSocket(this, std::move(sock));
            while(running_) {
                boost::asio::ip::udp::endpoint remote_endpoint;
                size_t len = udp_sock_->recvFrom(recv_buf_.data(), MAX_UDP_RECV_BUF, remote_endpoint, yield);
                if (!ec) {
                    std::unique_ptr<uint8_t[]> recv_data = std::unique_ptr<uint8_t[]>(new uint8_t[len]);
                    memcpy(recv_data.get(), recv_buf_.data(), len);
                    onUdpSocketRecv(udp_sock_, std::move(recv_data), len, remote_endpoint);
                }
            }
        });
        return 0;
    }

    void stopListen() {
        running_ = false;
    }
private:
    ThreadWorker *worker_ = nullptr;
    UdpSocket *udp_sock_ = nullptr;
    boost::array<uint8_t, MAX_UDP_RECV_BUF> recv_buf_;
    bool running_ = false;
    std::function<int32_t(std::shared_ptr<boost::asio::ip::udp::socket>, std::unique_ptr<uint8_t[]>, size_t)> cb_;
};
};