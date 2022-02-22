#pragma once
#include "server/udp/udp_server.hpp"

namespace mms {
class LiteICEServer : public UdpServer {
public:
    LiteICEServer(ThreadWorker *worker);
};
};