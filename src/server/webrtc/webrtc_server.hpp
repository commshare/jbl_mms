#pragma once
#include "server/udp/udp_server.hpp"

namespace mms {
class WebRtcServer : public UdpServer {
public:
    WebRtcServer(ThreadWorker *worker) : UdpServer(worker) {

    }
}
};