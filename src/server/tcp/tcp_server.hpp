#pragma once
#include <stdint.h>
#include <string>
#include <iostream>

#include <memory>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/spawn.hpp>

#include "base/thread/thread_pool.hpp"
#include "base/network/tcp_socket.hpp"

namespace mms {
template <typename CONN>
class ServerConnHandler {
public:
    virtual void onConnOpen(std::unique_ptr<CONN> conn) {};
    virtual void onConnClosed(std::unique_ptr<CONN> conn) {};
};

template <typename CONN>
class TcpServer {
public:
    TcpServer(ThreadWorker *worker):worker_(worker) {

    }

    virtual ~TcpServer() {

    }
public:
    void setConnHandler(ServerConnHandler<CONN> *handler) {
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
                    auto client_conn = std::unique_ptr<CONN>(new CONN(tcp_sock, worker, yield));
                    handler_->onConnOpen(std::move(client_conn));
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
    ServerConnHandler<CONN> *handler_;
    boost::shared_ptr<boost::asio::ip::tcp::acceptor> acceptor_;
};
};