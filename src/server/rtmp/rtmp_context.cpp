#include <iostream>
#include "rtmp_context.hpp"

namespace mms {

void RtmpContext::run() {
    char data[128];
    auto real_size = tcp_socket_->recvSome(data, 7);
    std::cout << "real size:" << real_size << std::endl;
    tcp_socket_->send(data, real_size);
}

};