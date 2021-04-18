#include <iostream>
#include "rtmp_context.hpp"

namespace mms {

void RtmpServerContext::run() {
    char c0c1[1537];
    std::cout << "wait for c0, c1" << std::endl;
    if(!tcp_socket_->recv(c0c1, 1537)) {
        tcp_socket_->close(); // 关闭socket
        return;
    }
    std::cout << "recv c0 c1" << std::endl;
    char *s0s1s2 = new char[3073];
    _genS0S1S2(c0c1, s0s1s2);
    // send s0, s1, s2
    if(!tcp_socket_->send(s0s1s2, 3073)) {
        tcp_socket_->close(); // 关闭socket
        delete s0s1s2;
        return;
    }

    char c2[1536];
    if(!tcp_socket_->recv(c2, 1536)) {
        tcp_socket_->close(); // 关闭socket
        delete s0s1s2;
        return;
    }

    int pos = 0;
    while(1) {
        int recv_size = tcp_socket_->recvSome(buffer_.data() + pos, buffer_.size() - pos);
        // decode data in buffer.
        std::cout << "recv some size:" << recv_size << std::endl;
    }
}

void RtmpServerContext::_genS0S1S2(char *c0c1, char *s0s1s2) {
    //s0
    s0s1s2[0] = '\x03';
    //s1
    memset(s0s1s2 + 1, 0, 8);
    //s2
    int32_t t = ntohl(*(int32_t*)(c0c1 + 1));
    *(int32_t*)(s0s1s2 + 1537) = htonl(t);
    *(int32_t*)(s0s1s2 + 1541) = htonl(time(NULL));
    memcpy(s0s1s2 + 1545, c0c1 + 9, 1528);
}

};