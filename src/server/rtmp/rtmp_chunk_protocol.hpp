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

#include "rtmp_define.hpp"
#include "rtmp_conn.hpp"
#include "rtmp_protocol.hpp"
namespace mms {
class RtmpChunkProtocol {
public:
    RtmpChunkProtocol(RtmpConn *conn):conn_(conn) {

    }

    virtual ~RtmpChunkProtocol() {

    }

    int32_t cycleRecvRtmpMessage(const std::function<int32_t(std::shared_ptr<RtmpMessage>)> & recv_handler) {
        recv_handler_ = recv_handler;

        while(1) {// todo reduce read system call, use recvSome
            // read basic header
            char d;
            bool ret = conn_->recv(&d, 1);
            if (!ret) {
                conn_->close();
                return;
            }
            
            int32_t cid = (int32_t)(d & 0x3f);
            int8_t fmt = (d >> 6) & 0x03;
            if (cid == 0) {
                if (!conn_->recv(&d, 1)) {
                    cid += 64;
                    cid += (int32_t)d;
                }
            } else if(cid == 1) {
                char buf[2];
                if (!conn_->recv(buf, 2)) {
                    conn_->close();
                    return;
                }
                cid = 64;
                cid += (int32_t)(buf[0]);
                cid += (int32_t)(buf[1]);
            }
            // read chunk message header
            auto cid_it = chunk_streams_.find(cid);
            std::shared_ptr<RtmpChunk> prev_chunk;
            if (cid_it != chunk_streams_.end()) {
                prev_chunk = cid_it->second;
            }
            // this chunk info
            // 优先从cache中获取chunk
            std::shared_ptr<RtmpChunk> chunk;
            auto cache_cid_it = chunk_cache_.find(cid);
            if (cache_cid_it != chunk_cache_.end()) {
                chunk = cache_cid_it->second;
                chunk_cache_.erase(cache_cid_it);
            }

            if (!chunk) {
                chunk = std::make_shared<RtmpChunk>();
            }

            if (fmt == 0) {
                char t[4] = {0};
                if(!conn_->recv(t+1, 3)) {
                    conn_->close();
                    return;
                }
                chunk->chunk_message_header_.timestamp_ = ntohl(*(int32_t*)t);

                memset(t, 0, 4);
                if(!conn_->recv(t+1, 3)) {
                    conn_->close();
                    return;
                }
                chunk->chunk_message_header_.message_length_ = ntohl(*(int32_t*)t);

                if(!conn_->recv((char*)&chunk->chunk_message_header_.message_type_id_, 1)) {
                    conn_->close();
                    return;
                }

                memset(t, 0, 4);
                if(!conn_->recv(t, 4)) {
                    conn_->close();
                    return;
                }
                chunk->chunk_message_header_.message_stream_id_ = ntohl(*(int32_t*)t);

                if (chunk->chunk_message_header_.timestamp_ == 0x00ffffff) {
                    memset(t, 0, 4);
                    if(!conn_->recv(t, 4)) {
                        conn_->close();
                        return;
                    }
                    chunk->chunk_message_header_.timestamp_ = ntohl(*(int32_t*)t);
                }
            } else if (fmt == 1) {
                if (!prev_chunk) {//type1 必须有前面的chunk作为基础
                    conn_->close();
                    return;
                }
                *chunk = *prev_chunk;

                char t[4] = {0};
                if(!conn_->recv(t+1, 3)) {
                    conn_->close();
                    return;
                }

                int32_t time_delta = ntohl(*(int32_t*)t);
                chunk->chunk_message_header_.timestamp_ = prev_chunk->chunk_message_header_.timestamp_ + time_delta;
                
                memset(t, 0, 4);
                if(!conn_->recv(t+1, 3)) {
                    conn_->close();
                    return;
                }
                chunk->chunk_message_header_.message_length_ = ntohl(*(int32_t*)t);

                if(!conn_->recv((char*)&chunk->chunk_message_header_.message_type_id_, 1)) {
                    conn_->close();
                    return;
                }

                chunk->chunk_message_header_.message_type_id_ = prev_chunk->chunk_message_header_.message_type_id_;
            } else if (fmt == 2) {
                if (!prev_chunk) {//type2 必须有前面的chunk作为基础
                    conn_->close();
                    return;
                }
                *chunk = *prev_chunk;

                char t[4] = {0};
                if(!conn_->recv(t+1, 3)) {
                    conn_->close();
                    return;
                }
                int32_t time_delta = ntohl(*(int32_t*)t);
                chunk->chunk_message_header_.timestamp_ = prev_chunk->chunk_message_header_.timestamp_ + time_delta;
            } else if (fmt == 3) {
                *chunk = *prev_chunk;
            }
            chunk_streams_[cid] = chunk;

            if (prev_chunk) {
                prev_chunk->clear();
                chunk_cache_[cid] = prev_chunk;
            }

            if (chunk->chunk_message_header_.message_length_ >= 2*1024*1024) {// packet too big
                conn_->close();
                return;
            }

            if (!chunk->rtmp_message_) {
                chunk->rtmp_message_ = std::make_shared<RtmpMessage>(chunk->chunk_message_header_.message_length_);
            }
            // read the payload
            int32_t this_chunk_payload_size = std::min(in_chunk_size_, chunk->chunk_message_header_.message_length_ - chunk->rtmp_message_->payload_size_);
            if(!conn_->recv(chunk->rtmp_message_->payload_ + chunk->rtmp_message_->payload_size_, this_chunk_payload_size)) {
                conn_->close();
                return;
            }

            chunk->rtmp_message_->payload_size_ += this_chunk_payload_size;
            // if we get a rtmp message
            if (chunk->rtmp_message_->payload_size_ == chunk->chunk_message_header_.message_length_) {
                // process this chunk->rtmp_message_;
                // todo fill rtmp message type...
                if (0 != recv_handler_(chunk->rtmp_message_)) {
                    conn_->close();
                    return;
                } 

                if (conn_->getRecvCount() >= window_ack_size_) {// send window ackledgement size message

                }
                // release this chunk->rtmp_message_
            }
        }
    }
private:
    RtmpConn *conn_;
    std::function<void(std::shared_ptr<RtmpMessage>)> recv_handler_;

    boost::array<char, 1024*1024> buffer_;
    int32_t in_chunk_size_ = 128;
    int32_t out_chunk_size_ = 128;
    std::unordered_map<uint32_t, std::shared_ptr<RtmpChunk>> chunk_streams_;
    std::unordered_map<uint32_t, std::shared_ptr<RtmpChunk>> chunk_cache_;
};
};
