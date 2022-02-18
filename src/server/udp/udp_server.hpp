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
            udp_sock_ = std::make_shared<boost::asio::ip::udp::socket>(worker_->getIOContext());
            boost::asio::ip::udp::endpoint local_endpoint(boost::asio::ip::address::from_string("0.0.0.0"), port);
            udp_sock_->open(boost::asio::ip::udp::v4());
            udp_sock_->set_option(boost::asio::ip::udp::socket::reuse_address(true));
            udp_sock_->bind(local_endpoint);
            
            if (!udp_sock_->is_open()) {
                return;
            }
            while(running_) {
                boost::asio::ip::udp::endpoint remote_endpoint;
                size_t len = udp_sock_->async_receive_from(boost::asio::buffer(recv_buf_.data(), MAX_UDP_RECV_BUF), remote_endpoint, yield[ec]);
                if (!ec) {
                    std::cout << "***************** udp recv: " << len << " ********************" << std::endl;
                    cb_(udp_sock_, (const uint8_t*)recv_buf_.data(), len);
                }
            }
        });
        return 0;
    }

    void onRecvPkt(const std::function<int32_t(std::shared_ptr<boost::asio::ip::udp::socket>, const uint8_t*, size_t)> & cb) {
        cb_ = cb;
    }

    void stopListen() {
        running_ = false;
    }
private:
    ThreadWorker *worker_ = nullptr;
    std::shared_ptr<boost::asio::ip::udp::socket> udp_sock_;
    boost::array<uint8_t, MAX_UDP_RECV_BUF> recv_buf_;
    bool running_ = false;
    std::function<int32_t(std::shared_ptr<boost::asio::ip::udp::socket>, const uint8_t*, size_t)> cb_;
};
};