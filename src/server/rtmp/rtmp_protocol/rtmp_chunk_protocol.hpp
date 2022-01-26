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

#include "rtmp_define.hpp"
#include "server/rtmp/rtmp_conn/rtmp_conn.hpp"
#include "server/rtmp/rtmp_message/chunk_message/rtmp_set_chunk_size_message.hpp"
#include "server/rtmp/rtmp_message/chunk_message/rtmp_abort_message.hpp"

namespace mms {
class RtmpChunkProtocol {
public:
    RtmpChunkProtocol(RtmpConn *conn):conn_(conn) {
        for (uint32_t cid = 0; cid < 256; cid++) {
            recv_chunk_cache_[cid] = std::make_shared<RtmpChunk>();
        }
    }

    virtual ~RtmpChunkProtocol() {

    }

    template<typename T>
    bool sendRtmpMessage(const T & msg) {
        std::shared_ptr<RtmpMessage> rtmp_msg = msg.encode();
        return _sendRtmpMessage(rtmp_msg);
    }

    bool _sendRtmpMessage(std::shared_ptr<RtmpMessage> rtmp_msg) {
        size_t left_size = rtmp_msg->payload_size_;
        size_t cur_pos = 0;
        uint8_t fmt = RTMP_FMT_TYPE0;
        while (left_size > 0) {
            // 判断fmt类型
            auto prev_chunk = send_chunk_streams_[rtmp_msg->chunk_stream_id_];
            if (prev_chunk) {
                // 不使用fmt3
                if (rtmp_msg->message_stream_id_ == prev_chunk->rtmp_message_->message_stream_id_) {
                    fmt = RTMP_FMT_TYPE1;
                    if (rtmp_msg->timestamp_ < prev_chunk->rtmp_message_->timestamp_) {
                        fmt = RTMP_FMT_TYPE0;
                    } else if (rtmp_msg->payload_size_ == prev_chunk->rtmp_message_->payload_size_) {
                        fmt = RTMP_FMT_TYPE2;
                    }
                }
            }
            // +--------------+----------------+--------------------+--------------+
            //  | Basic Header | Message Header | Extended Timestamp | Chunk Data |
            //  +--------------+----------------+--------------------+--------------+
            // 发送basic header
            std::shared_ptr<RtmpChunk> chunk = std::make_shared<RtmpChunk>();
            chunk->rtmp_message_ = rtmp_msg;
            // Chunk stream IDs 2-63 can be encoded in the 1-byte version of this field.
            std::cout << "rtmp_msg->chunk_stream_id_:" << (uint32_t)rtmp_msg->chunk_stream_id_  << std::endl;
            if (rtmp_msg->chunk_stream_id_ >= 2 && rtmp_msg->chunk_stream_id_ <= 63) {
                uint8_t d = ((fmt&0x03)<<6) | (rtmp_msg->chunk_stream_id_&0x3f);
                if (!conn_->send(&d, 1)) {
                    conn_->close();
                    return false;
                }
            } else if (rtmp_msg->chunk_stream_id_ >= 64 && rtmp_msg->chunk_stream_id_ <= 319) {
                uint8_t buf[2];
                buf[0] = ((fmt&0x03)<<6) | 0x00;
                buf[1] = (rtmp_msg->chunk_stream_id_ - 64) & 0xff;
                if (!conn_->send(buf, 2)) {
                    conn_->close();
                    return false;
                }
            } else if (rtmp_msg->chunk_stream_id_ >= 64 && rtmp_msg->chunk_stream_id_ <= 65599) {//这里真是奇葩，64有重叠
                uint8_t buf[3];
                buf[0] = ((fmt&0x03)<<6) | 0x01;
                auto csid = rtmp_msg->chunk_stream_id_ - 64;
                buf[1] = (csid%256) & 0xff;
                buf[2] = (csid/256) & 0xff;
                if (!conn_->send(buf, 3)) {
                    conn_->close();
                    return false;
                }
            }
            // 发送message header
            int this_chunk_payload_size = std::min(out_chunk_size_, left_size);
            if (fmt == RTMP_FMT_TYPE0) {
                uint32_t t = htonl(rtmp_msg->timestamp_&0xffffff);
                if(!conn_->send((uint8_t*)&t + 1, 3)) {
                    conn_->close();
                    return false;
                }

                t = htonl(rtmp_msg->payload_size_);
                if(!conn_->send((uint8_t*)&t + 1, 3)) {
                    conn_->close();
                    return false;
                }
                

                if(!conn_->send(&rtmp_msg->message_type_id_, 1)) {
                    conn_->close();
                    return false;
                }

                t = htonl(rtmp_msg->message_stream_id_);
                if(!conn_->send((uint8_t*)&t, 4)) {
                    conn_->close();
                    return false;
                }
            } else if (fmt == RTMP_FMT_TYPE1) {
                uint32_t timestamp_delta = rtmp_msg->timestamp_ - prev_chunk->rtmp_message_->timestamp_;
                uint32_t t = htonl(timestamp_delta);
                if(!conn_->send((uint8_t*)&t + 1, 3)) {
                    conn_->close();
                    return false;
                }

                t = htonl(rtmp_msg->payload_size_);
                if(!conn_->send((uint8_t*)&t + 1, 3)) {
                    conn_->close();
                    return false;
                }
                
                if(!conn_->send(&rtmp_msg->message_type_id_, 1)) {
                    conn_->close();
                    return false;
                }
            } else if (fmt == RTMP_FMT_TYPE2) {
                uint32_t timestamp_delta = rtmp_msg->timestamp_ - prev_chunk->rtmp_message_->timestamp_;
                uint32_t t = htonl(timestamp_delta);
                if(!conn_->send((uint8_t*)&t + 1, 3)) {
                    conn_->close();
                    return false;
                }
            } else if (fmt == RTMP_FMT_TYPE3) {// no header

            }
            // 发送exttimestamp
            if (rtmp_msg->timestamp_ >= 0x00ffffff) {
                uint32_t t = htonl(rtmp_msg->timestamp_);
                if(!conn_->send((uint8_t*)&t, 4)) {
                    conn_->close();
                    return false;
                }
            }
            // 发送chunk data
            std::cout << "this_chunk_payload_size:" << this_chunk_payload_size << std::endl;
            if (!conn_->send(rtmp_msg->payload_ + cur_pos, this_chunk_payload_size)) {
                conn_->close();
                return false;
            }

            left_size -= this_chunk_payload_size;
            cur_pos += this_chunk_payload_size;
            // 发送结束，记录本次发送的chunk
            chunk->chunk_payload_size_ = this_chunk_payload_size;
            send_chunk_streams_[rtmp_msg->chunk_stream_id_] = chunk;
        }
        return true;
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
            if (cid == 0) {
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
                cid += (int32_t)(buf[1])*256;
            }
            // read chunk message header
            auto cid_it = recv_chunk_streams_.find(cid);
            std::shared_ptr<RtmpChunk> prev_chunk;
            if (cid_it != recv_chunk_streams_.end()) {
                prev_chunk = cid_it->second;
            }
            // this chunk info
            // 优先从cache中获取chunk
            std::shared_ptr<RtmpChunk> chunk;
            auto cache_cid_it = recv_chunk_cache_.find(cid);
            if (cache_cid_it != recv_chunk_cache_.end()) {
                chunk = cache_cid_it->second;
                recv_chunk_cache_.erase(cache_cid_it);
            }

            if (!chunk) {
                chunk = std::make_shared<RtmpChunk>();
            }

            if (fmt == RTMP_FMT_TYPE0) {
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
            } else if (fmt == RTMP_FMT_TYPE1) {
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
            } else if (fmt == RTMP_FMT_TYPE2) {
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
            } else if (fmt == RTMP_FMT_TYPE3) {
                *chunk = *prev_chunk;
            }
            recv_chunk_streams_[cid] = chunk;

            if (prev_chunk) {
                prev_chunk->clear();
                recv_chunk_cache_[cid] = prev_chunk;
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
                chunk->rtmp_message_->chunk_stream_id_ = cid;
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

    inline size_t getOutChunkSize() {
        return out_chunk_size_;
    }

    inline void setOutChunkSize(size_t s) {
        out_chunk_size_ = s;
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
        auto it = recv_chunk_streams_.find(cmd.chunk_id_);
        if (it != recv_chunk_streams_.end()) {
            recv_chunk_streams_.erase(it);
        }
        return true;
    }
private:
    RtmpConn *conn_;
    std::function<int32_t(std::shared_ptr<RtmpMessage>)> recv_handler_;

    boost::array<uint8_t, 1024*1024> recv_buffer_;
    boost::array<uint8_t, 1024*1024> send_buffer_;

    int32_t in_chunk_size_ = 128;
    size_t out_chunk_size_ = 128;
    std::unordered_map<uint32_t, std::shared_ptr<RtmpChunk>> recv_chunk_streams_;
    std::unordered_map<uint32_t, std::shared_ptr<RtmpChunk>> recv_chunk_cache_;
    std::unordered_map<uint32_t, std::shared_ptr<RtmpChunk>> send_chunk_streams_;

    std::vector<std::shared_ptr<RtmpMessage>> sending_rtmp_msgs_;
};
};
