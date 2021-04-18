#pragma once
#include <memory>
#include <unordered_map>
#include <boost/asio/ip/tcp.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>

#include "base/thread/thread_worker.hpp"
#include "base/network/tcp_socket.hpp"

#include "rtmp_protocol.hpp"
namespace mms {
enum RtmpState {
    RTMP_SERVER_STATE_WAIT_C0_C1    = 0,
    RTMP_SERVER_STATE_WAIT_C2       = 1,
    RTMP_SERVER_STATE_RECV_AV       = 2,
};

class RtmpServerContext {
public:
    RtmpServerContext(boost::shared_ptr<TcpSocket> sock) {
        tcp_socket_ = sock;
    }

    void run();
private:
    void _genS0S1S2(char *c0c1, char *s0s1s2);
    ThreadWorker *worker_;
    boost::shared_ptr<TcpSocket> tcp_socket_;
    boost::array<char, 1024*1024> buffer_;
    std::unordered_map<uint32_t, std::shared_ptr<RtmpChunk>> chunk_streams_;
    boost::shared_ptr<RtmpMessage> rtmp_message_;
};
};