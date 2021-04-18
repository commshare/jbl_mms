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
        char d;
        bool ret = tcp_socket_->recv(&d, 1);
        if (!ret) {
            tcp_socket_->close();
            return;
        }
        int32_t cid = (int32_t)(d & 0x3f);
        int8_t fmt = (d >> 6) & 0x03;
        if (cid > 1) {
            // todo read message header
        }

        if (cid == 0) {
            if (!tcp_socket_->recv(&d, 1)) {
                cid += 64;
                cid += (int32_t)d;
            }
        } else if(cid == 1) {
            char buf[2];
            if (!tcp_socket_->recv(buf, 2)) {
                tcp_socket_->close();
                return;
            }
            cid = 64;
            cid += (int32_t)(buf[0]);
            cid += (int32_t)(buf[1]);
        }
        // recv message header

        auto cid_it = chunk_streams_.find(cid);
        std::shared_ptr<RtmpChunk> chunk;
        if (cid_it == chunk_streams_.end()) {
            chunk = std::make_shared<RtmpChunk>();
            chunk_streams_[cid] = chunk;
        } else {
            chunk = cid_it->second;
        }

        std::cout << "cid:" << (uint32_t)cid << ", fmt:" << (uint32_t)fmt << std::endl;
        sleep(10000);
        // if (!rtmp_message_->rtmp_message_) {// chunk 的rtmp message 为空

        // }



        // decode data in buffer.
        // std::cout << "recv some size:" << recv_size << std::endl;
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