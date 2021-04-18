#include <iostream>
#include <atomic>

#include <boost/asio.hpp>  
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/asio/thread_pool.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>

#include "base/thread/thread_pool.hpp"
#include "base/jemalloc/je_new.h"

#include "server/rtmp/rtmp_server.hpp"

using namespace mms;

void waitExit() {
    std::atomic_bool exit(false);
    boost::asio::io_context io;
    boost::asio::signal_set sigset(io, SIGINT, SIGTERM);
    sigset.async_wait([&exit](const boost::system::error_code& err, int signal) {
        exit = true;
    });

    boost::system::error_code ec;
    io.run(ec);
    while(1) {
        if (exit) {
            break;
        }
        sleep(1000);
    }  
}

int main(int argc, char *argv[]) {
    thread_pool_inst::get_mutable_instance().start(std::thread::hardware_concurrency());

    RtmpServer rtmp_server(thread_pool_inst::get_mutable_instance().getWorker(-1));
    if(!rtmp_server.start()) {
        return -1;
    }

    waitExit();

    std::cout << "stop rtmp server" << std::endl;
    rtmp_server.stop();
    thread_pool_inst::get_mutable_instance().stop();
    return 0;
}