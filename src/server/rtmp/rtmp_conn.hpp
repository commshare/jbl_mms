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
class RtmpConn : public TcpSocket {
public:
    RtmpConn(boost::asio::ip::tcp::socket *sock, ThreadWorker *worker, boost::asio::yield_context y):TcpSocket(sock, worker, y) {
    }

    void doService();
private:
    void _genS0S1S2(char *c0c1, char *s0s1s2);
    boost::array<char, 1024*1024> buffer_;
    std::unordered_map<uint32_t, std::shared_ptr<RtmpChunk>> chunk_streams_;
    boost::shared_ptr<RtmpMessage> rtmp_message_;

    int32_t in_chunk_size_ = 128;
};
};