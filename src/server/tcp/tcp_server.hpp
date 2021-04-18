#pragma once
#include <stdint.h>
#include <string>
#include <iostream>

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/spawn.hpp>

#include "base/thread/thread_pool.hpp"
#include "base/network/tcp_socket.hpp"

namespace mms {
class TcpServerHandler {
public:
    virtual void onTcpSocketOpen(boost::shared_ptr<TcpSocket> sock) {};
    virtual void onTcpSocketClosed(boost::shared_ptr<TcpSocket> sock) {};
};

class TcpServer {
public:
    TcpServer(ThreadWorker *worker):worker_(worker) {

    }

    virtual ~TcpServer() {

    }
public:
    void setTcpHandler(TcpServerHandler *handler) {
        handler_ = handler;
    }

    int32_t startListen(uint16_t port, const std::string & addr = "") {
        if (!worker_) {
            return -1;
        }

        boost::asio::spawn(worker_->getIOContext(), [port, addr, this](boost::asio::yield_context yield) {
            boost::asio::ip::tcp::endpoint endpoint;
            endpoint.port(port);
            if (!addr.empty()) {
                endpoint.address(boost::asio::ip::address::from_string(addr));
            } else {
                endpoint.address(boost::asio::ip::address::from_string("0.0.0.0"));
            }

            acceptor_ = boost::make_shared<boost::asio::ip::tcp::acceptor>(worker_->getIOContext(), endpoint);
            acceptor_->set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
            while(1) {
                boost::system::error_code ec;
                auto worker = thread_pool_inst::get_mutable_instance().getWorker(-1);
                boost::asio::ip::tcp::socket *tcp_sock = new boost::asio::ip::tcp::socket(worker->getIOContext());
                acceptor_->async_accept(*tcp_sock, yield[ec]);
                if (ec) {
                    delete tcp_sock;
                    tcp_sock = nullptr;
                    break;
                }
                
                boost::asio::spawn(worker->getIOContext(), [this, tcp_sock, worker](boost::asio::yield_context yield) {
                    auto client_sock = boost::make_shared<TcpSocket>(tcp_sock, worker, yield);
                    handler_->onTcpSocketOpen(client_sock);
                });
            }
        });

        return 0;
    }

    void stopListen() {
        worker_->dispatch([this]{
            acceptor_->close();
            acceptor_.reset();
        });
    }
private:
    ThreadWorker *worker_;
    TcpServerHandler *handler_;
    boost::shared_ptr<boost::asio::ip::tcp::acceptor> acceptor_;
};
};