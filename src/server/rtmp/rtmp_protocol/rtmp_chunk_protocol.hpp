/*
MIT License

Copyright (c) 2021 jiangbaolin

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#pragma once
#include <functional>
#include <unordered_map>
#include <vector>
#include <list>
#include <condition_variable>
#include <mutex>

#include "base/shared_ptr.hpp"

#include "rtmp_define.hpp"
#include "server/rtmp/rtmp_conn/rtmp_conn.hpp"
#include "server/rtmp/rtmp_message/chunk_message/rtmp_set_chunk_size_message.hpp"
#include "server/rtmp/rtmp_message/chunk_message/rtmp_abort_message.hpp"

namespace mms {
class RtmpChunkProtocol {
public:
    RtmpChunkProtocol(RtmpConn *conn);
    virtual ~RtmpChunkProtocol();
    template<typename T>
    bool sendRtmpMessage(const T & msg, boost::asio::yield_context & yield) {
        std::shared_ptr<RtmpMessage> rtmp_msg = msg.encode();
        if (!rtmp_msg) {
            return false;
        }

        while (sending_) {
            boost::asio::steady_timer timer(conn_->getWorker()->getIOContext());
            timer.expires_from_now(std::chrono::milliseconds(10));
            timer.async_wait(yield);
        }

        sending_ = true;
        auto ret = _sendRtmpMessage(rtmp_msg, yield);
        sending_ = false;
        send_handler_();
        return ret;
    }

    bool _sendRtmpMessage(std::shared_ptr<RtmpMessage> rtmp_msg, boost::asio::yield_context & yield);
    // 异步方式发送
    void sendRtmpMessage(std::shared_ptr<RtmpMessage> rtmp_msg);
    int32_t cycleRecvRtmpMessage(const std::function<bool(std::shared_ptr<RtmpMessage>, boost::asio::yield_context &)> & recv_handler, boost::asio::yield_context & yield);

    size_t getOutChunkSize();
    void setOutChunkSize(size_t s);
    void close();
private:
    std::function<void()> send_handler_;
    bool handleSetChunkSize(std::shared_ptr<RtmpMessage> msg);
    bool handleAbort(std::shared_ptr<RtmpMessage> msg);
private:
    RtmpConn *conn_;
    std::function<bool(std::shared_ptr<RtmpMessage>, boost::asio::yield_context & yield)> recv_handler_;

    boost::array<uint8_t, 1024*1024> recv_buffer_;
    boost::array<uint8_t, 1024*1024> send_buffer_;

    int32_t in_chunk_size_ = 128;
    size_t out_chunk_size_ = 128;
    std::unordered_map<uint32_t, std::shared_ptr<RtmpChunk>> recv_chunk_streams_;
    std::unordered_map<uint32_t, std::shared_ptr<RtmpChunk>> recv_chunk_cache_;
    std::unordered_map<uint32_t, std::shared_ptr<RtmpChunk>> send_chunk_streams_;

    std::list<std::shared_ptr<RtmpMessage>> sending_rtmp_msgs_;
    std::atomic<bool> sending_{false};
};
};
