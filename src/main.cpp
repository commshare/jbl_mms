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

    uint8_t clientRandom[32] = {
        0x36, 0xc1, 0x29, 0xd0, 0x1a, 0x32, 0x00, 0x89, 0x4b, 0x91, 0x79, 0xfa, 0xac, 0x58, 0x9d, 0x98, 
        0x35, 0xd5, 0x87, 0x75, 0xf9, 0xb5, 0xea, 0x35, 0x87, 0xcb, 0x8f, 0xd0, 0x36, 0x4c, 0xae, 0x8c};

    uint8_t serverRandom[32] = {
        0xf6, 0xc9, 0x57, 0x5e, 0xd7, 0xdd, 0xd7, 0x3e, 0x1f, 0x7d, 0x16, 0xec, 0xa1, 0x15, 0x41, 0x58,
        0x12, 0xa4, 0x3c, 0x2b, 0x74, 0x7d, 0xaa, 0xaa, 0xe0, 0x43, 0xab, 0xfb, 0x50, 0x05, 0x3f, 0xce};

    uint8_t preMaster[48] = {
        0xf8, 0x93, 0x8e, 0xcc, 0x9e, 0xde, 0xbc, 0x50, 0x30, 0xc0, 0xc6, 0xa4, 0x41, 0xe2, 0x13, 0xcd, 
        0x24, 0xe6, 0xf7, 0x70, 0xa5, 0x0d, 0xda, 0x07, 0x87, 0x6f, 0x8d, 0x55, 0xda, 0x06, 0x2b, 0xca, 
        0xdb, 0x38, 0x6b, 0x41, 0x1f, 0xd4, 0xfe, 0x43, 0x13, 0xa6, 0x04, 0xfc, 0xe6, 0xc1, 0x7f, 0xbc};

    std::string seed3;
    seed3.append((char *)clientRandom, 32);
    seed3.append((char *)serverRandom, 32);
    std::string master_secret = PRF(std::string((char *)preMaster, 48), "master secret", seed3, 48);
    printf("master secret:\r\n");
    for (int i = 0; i < master_secret.size(); i++)
    {
        printf("%02x ", (uint8_t)master_secret[i]);
    }
    printf("\r\n");

    waitExit();

    std::cout << "stop rtmp server" << std::endl;
    http_server.stop();
    rtmp_server.stop();
    webrtc_server.stop();
    thread_pool_inst::get_mutable_instance().stop();
    return 0;
}