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
#include "server/rtmp/rtmp_conn/rtmp_conn.hpp"
#include "server/rtmp/rtmp_message/chunk_message/rtmp_set_chunk_size_message.hpp"
#include "server/rtmp/rtmp_message/chunk_message/rtmp_abort_message.hpp"

namespace mms {
class RtmpChunkProtocol {
public:
    RtmpChunkProtocol(RtmpConn *conn):conn_(conn) {
        for (uint32_t cid = 0; cid < 256; cid++) {
            chunk_cache_[cid] = std::make_shared<RtmpChunk>();
        }
    }

    virtual ~RtmpChunkProtocol() {

    }

    int32_t cycleRecvRtmpMessage(const std::function<int32_t(std::shared_ptr<RtmpMessage>)> & recv_handler) {
        recv_handler_ = recv_handler;

        while(1) {// todo reduce read system call, use recvSome
            // read basic header
            uint8_t d;
            bool ret = conn_->recv(&d, 1);
            if (!ret) {
                conn_->close();
                return -1;
            }
            
            int32_t cid = (int32_t)(d & 0x3f);
            int8_t fmt = (d >> 6) & 0x03;
            if (cid == RTMP_FMT_TYPE0) {
                if (!conn_->recv(&d, 1)) {
                    cid += 64;
                    cid += (int32_t)d;
                }
            } else if(cid == 1) {
                uint8_t buf[2];
                if (!conn_->recv(buf, 2)) {
                    conn_->close();
                    return -2;
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
                uint8_t t[4] = {0};
                if(!conn_->recv(t+1, 3)) {
                    conn_->close();
                    return -3;
                }
                chunk->chunk_message_header_.timestamp_ = ntohl(*(int32_t*)t);

                memset(t, 0, 4);
                if(!conn_->recv(t+1, 3)) {
                    conn_->close();
                    return -4;
                }
                chunk->chunk_message_header_.message_length_ = ntohl(*(int32_t*)t);

                if(!conn_->recv((uint8_t*)&chunk->chunk_message_header_.message_type_id_, 1)) {
                    conn_->close();
                    return -5;
                }

                memset(t, 0, 4);
                if(!conn_->recv(t, 4)) {
                    conn_->close();
                    return -6;
                }
                chunk->chunk_message_header_.message_stream_id_ = ntohl(*(int32_t*)t);

                if (chunk->chunk_message_header_.timestamp_ == 0x00ffffff) {
                    memset(t, 0, 4);
                    if(!conn_->recv(t, 4)) {
                        conn_->close();
                        return -7;
                    }
                    chunk->chunk_message_header_.timestamp_ = ntohl(*(int32_t*)t);
                }
            } else if (fmt == 1) {
                if (!prev_chunk) {//type1 必须有前面的chunk作为基础
                    conn_->close();
                    return -8;
                }
                *chunk = *prev_chunk;

                uint8_t t[4] = {0};
                if(!conn_->recv(t+1, 3)) {
                    conn_->close();
                    return -9;
                }

                int32_t time_delta = ntohl(*(int32_t*)t);
                chunk->chunk_message_header_.timestamp_ = prev_chunk->chunk_message_header_.timestamp_ + time_delta;
                
                memset(t, 0, 4);
                if(!conn_->recv(t+1, 3)) {
                    conn_->close();
                    return -10;
                }
                chunk->chunk_message_header_.message_length_ = ntohl(*(int32_t*)t);

                if(!conn_->recv((uint8_t*)&chunk->chunk_message_header_.message_type_id_, 1)) {
                    conn_->close();
                    return -11;
                }

                chunk->chunk_message_header_.message_type_id_ = prev_chunk->chunk_message_header_.message_type_id_;
            } else if (fmt == 2) {
                if (!prev_chunk) {//type2 必须有前面的chunk作为基础
                    conn_->close();
                    return -12;
                }
                *chunk = *prev_chunk;

                uint8_t t[4] = {0};
                if(!conn_->recv(t+1, 3)) {
                    conn_->close();
                    return -13;
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
                return -14;
            }

            if (!chunk->rtmp_message_) {
                chunk->rtmp_message_ = std::make_shared<RtmpMessage>(chunk->chunk_message_header_.message_length_);
            }
            // read the payload
            int32_t this_chunk_payload_size = std::min(in_chunk_size_, chunk->chunk_message_header_.message_length_ - chunk->rtmp_message_->payload_size_);
            if(!conn_->recv(chunk->rtmp_message_->payload_ + chunk->rtmp_message_->payload_size_, this_chunk_payload_size)) {
                conn_->close();
                return -15;
            }

            chunk->rtmp_message_->payload_size_ += this_chunk_payload_size;
            // if we get a rtmp message
            if (chunk->rtmp_message_->payload_size_ == chunk->chunk_message_header_.message_length_) {
                // todo fill rtmp message type...
                chunk->rtmp_message_->timestamp_ = chunk->chunk_message_header_.timestamp_;
                chunk->rtmp_message_->message_type_id_ = chunk->chunk_message_header_.message_type_id_;
                chunk->rtmp_message_->message_stream_id_ = chunk->chunk_message_header_.message_stream_id_;
                // set chunk size and abort message command process in chunk level
                if (chunk->rtmp_message_->message_type_id_ == RTMP_MESSAGE_TYPE_SET_CHUNK_SIZE) {  
                    if (!handleSetChunkSize(chunk->rtmp_message_)) {
                        conn_->close();
                        return -16;
                    }       
                    continue;
                } else if (chunk->rtmp_message_->message_type_id_ == RTMP_MESSAGE_TYPE_ABORT_MESSAGE) {
                    if (!handleAbort(chunk->rtmp_message_)) {
                        conn_->close();
                        return -17;
                    }       
                    continue;
                }

                if (0 != recv_handler_(chunk->rtmp_message_)) {
                    conn_->close();
                    return -18;
                } 
            }
        }
    }
private:
    bool handleSetChunkSize(std::shared_ptr<RtmpMessage> msg) {
        RtmpSetChunkSizeMessage cmd;
        int ret = cmd.decode(msg);
        if (ret <= 0) {// 解析成功时，返回解析成功字节数
            return false;
        }
        in_chunk_size_ = cmd.chunk_size_;
        return true;
    }

    bool handleAbort(std::shared_ptr<RtmpMessage> msg) {
        RtmpAbortMessage cmd;
        int ret = cmd.decode(msg);
        if (ret <= 0) {// 解析成功时，返回解析成功字节数
            return false;
        }
        auto it = chunk_streams_.find(cmd.chunk_id_);
        if (it != chunk_streams_.end()) {
            chunk_streams_.erase(it);
        }
        return true;
    }
private:
    RtmpConn *conn_;
    std::function<int32_t(std::shared_ptr<RtmpMessage>)> recv_handler_;

    boost::array<char, 1024*1024> buffer_;
    int32_t in_chunk_size_ = 128;
    int32_t out_chunk_size_ = 128;
    std::unordered_map<uint32_t, std::shared_ptr<RtmpChunk>> chunk_streams_;
    std::unordered_map<uint32_t, std::shared_ptr<RtmpChunk>> chunk_cache_;
};
};
