#pragma once
#include <stdint.h>
#include <string>
#include <iostream>
#include <memory>

#include <boost/array.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/asio/spawn.hpp>

#include "base/thread/thread_pool.hpp"
#include "base/network/tcp_socket.hpp"

namespace mms {
#define MAX_UDP_RECV_BUF 2*1024*1024
class UdpServer {
public:
    UdpServer(ThreadWorker *worker):worker_(worker) {

    }

    int32_t startListen(uint16_t port) {
        if (!worker_) {
            return -1;
        }

        boost::asio::spawn(worker_->getIOContext(), [this, port](boost::asio::yield_context yield) {
            running_ = true;
            boost::system::error_code ec;
            boost::array<uint8_t, MAX_UDP_RECV_BUF> recv_buf;
            while(running_) {
                boost::asio::ip::udp::socket udp_sock(worker_->getIOContext());
                boost::asio::ip::udp::endpoint sender_endpoint;
                size_t len = udp_sock.async_receive_from(boost::asio::buffer(recv_buf), sender_endpoint, yield[ec]);
                cb_(std::move(udp_sock), (const uint8_t*)recv_buf.data(), len);
            }
        });
        return 0;
    }

    int32_t onRecvPkt(const std::function<int32_t(boost::asio::ip::udp::socket, const uint8_t*, size_t)> & cb) {
        cb_ = cb;
    }

    void stop() {
        running_ = false;
    }
private:
    ThreadWorker *worker_ = nullptr;
    bool running_ = false;
    std::function<int32_t(boost::asio::ip::udp::socket, const uint8_t*, size_t)> cb_;
};
};