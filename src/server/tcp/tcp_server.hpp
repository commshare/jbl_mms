#pragma once
#include <stdint.h>
#include <string>
#include <iostream>

#include <memory>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/spawn.hpp>

#include "base/thread/thread_pool.hpp"
#include "base/network/tcp_socket.hpp"
#include <boost/asio.hpp>
#include <boost/asio/experimental/as_tuple.hpp>
#include <boost/asio/experimental/awaitable_operators.hpp>
#include <boost/asio/experimental/channel.hpp>
#include <iostream>

using boost::asio::awaitable;
using boost::asio::buffer;
using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::experimental::as_tuple;
using boost::asio::experimental::channel;
using boost::asio::io_context;
using boost::asio::ip::tcp;
using boost::asio::steady_timer;
using boost::asio::use_awaitable;
namespace this_coro = boost::asio::this_coro;
using namespace boost::asio::experimental::awaitable_operators;
using namespace std::literals::chrono_literals;

namespace mms {
template <typename CONN>
class TcpServer : public TcpSocketHandler {
public:
    TcpServer(ThreadWorker *worker):worker_(worker) {

    }

    virtual ~TcpServer() {

    }
public:
    int32_t startListen(uint16_t port, const std::string & addr = "") {
        if (!worker_) {
            return -1;
        }

        boost::asio::co_spawn(worker_->getIOContext(), ([port, addr, this]()->boost::asio::awaitable<void>{
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
                auto worker = thread_pool_inst::get_mutable_instance().getWorker(-1);
                auto [ec, tcp_sock] = co_await acceptor_->async_accept(worker->getIOContext(), boost::asio::experimental::as_tuple(boost::asio::use_awaitable));
                if (!ec) {
                    auto ex = tcp_sock.get_executor();
                    auto client_conn = new CONN(this, std::move(tcp_sock), worker);
                    client_conn->open();
                } else {
                    steady_timer timer(co_await this_coro::executor);
                    timer.expires_after(100ms);
                    co_await timer.async_wait(boost::asio::use_awaitable);
                }
            }
        }), boost::asio::detached);
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
    boost::shared_ptr<boost::asio::ip::tcp::acceptor> acceptor_;
};
};