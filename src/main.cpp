#include <iostream>
#include <atomic>

#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/asio/thread_pool.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/program_options.hpp>

#include "base/thread/thread_pool.hpp"
#include "base/jemalloc/je_new.h"

#include "server/rtmp/rtmp_server.hpp"
#include "server/http/http_server.hpp"
#include "server/udp/udp_server.hpp"
#include "server/stun/stun_server.hpp"
#include "server/webrtc/webrtc_server.hpp"

#include "base/utils/utils.h"
#include "server/dtls/tls_prf.h"

using namespace mms;

void waitExit()
{
    std::atomic_bool exit(false);
    boost::asio::io_context io;
    boost::asio::signal_set sigset(io, SIGINT, SIGTERM);
    sigset.async_wait([&exit](const boost::system::error_code &err, int signal)
                      { exit = true; });

    boost::system::error_code ec;
    io.run(ec);
    while (1)
    {
        if (exit)
        {
            break;
        }
        sleep(1000);
    }
}

int main(int argc, char *argv[])
{
    boost::program_options::options_description opts("all options");
    opts.add_options()("c", boost::program_options::value<std::string>(), "the config file")("help", "mms is a multi media server.");
    boost::program_options::variables_map vm;
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, opts), vm);
    boost::program_options::notify(vm);

    if (vm.count("help"))
    { //若参数中有help选项
        std::cout << opts << std::endl;
        return -1;
    }
    else
    {
        if (!vm.count("c"))
        {
            std::cerr << "please set the config file." << std::endl;
            return -2;
        }
    }

    std::string config_file = vm["c"].as<std::string>();

    thread_pool_inst::get_mutable_instance().start(std::thread::hardware_concurrency());

    thread_pool_inst::get_mutable_instance().getWorker(RAND_WORKER)->delayInvoke([]() {
        
    }, 10);
    
    RtmpServer rtmp_server(thread_pool_inst::get_mutable_instance().getWorker(RAND_WORKER));
    if (!rtmp_server.start())
    {
        return -1;
    }

    HttpServer http_server(thread_pool_inst::get_mutable_instance().getWorker(RAND_WORKER));
    if (!http_server.start())
    {
        return -2;
    }

    StunServer stun_server(thread_pool_inst::get_mutable_instance().getWorker(RAND_WORKER));
    if (!stun_server.start())
    {
        std::cout << "start stun server failed." << std::endl;
        return -3;
    }

    WebRtcServer webrtc_server(thread_pool_inst::get_mutable_instance().getWorker(RAND_WORKER));
    if (!webrtc_server.start())
    {
        std::cout << "start webrtc server failed." << std::endl;
        return -4;
    }

    waitExit();

    std::cout << "stop rtmp server" << std::endl;
    http_server.stop();
    rtmp_server.stop();
    webrtc_server.stop();
    thread_pool_inst::get_mutable_instance().stop();
    return 0;
}