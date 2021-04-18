#pragma once
#include "base/network/tcp_socket.hpp"

namespace mms {
class RtmpHandshake {
public:
    RtmpHandshake(TcpSocket *sock) {
        data_ = new uint8_t[3073];
    }

    bool recvC0C1() {
        if(!sock_->recv(data_, 1537)) {
            return false;
        }
        return true;
    }

    bool sendS0S1S2() {
        // sock_->send()
    }
private:
    uint8_t *data_;
    TcpSocket *sock_;
};
};