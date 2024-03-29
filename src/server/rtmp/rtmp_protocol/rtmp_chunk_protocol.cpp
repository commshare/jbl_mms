#include "rtmp_chunk_protocol.hpp"
using namespace mms;

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

#include "base/shared_ptr.hpp"

#include "rtmp_define.hpp"
#include "server/rtmp/rtmp_conn/rtmp_conn.hpp"
#include "server/rtmp/rtmp_message/chunk_message/rtmp_set_chunk_size_message.hpp"
#include "server/rtmp/rtmp_message/chunk_message/rtmp_abort_message.hpp"

RtmpChunkProtocol::RtmpChunkProtocol(RtmpConn *conn):conn_(conn) {
    for (uint32_t cid = 0; cid < 256; cid++) {
        recv_chunk_cache_[cid] = std::make_shared<RtmpChunk>();
    }

    send_handler_ = [this]() {
        if (sending_rtmp_msgs_.empty()) {
            return;
        }

        if (sending_) {
            return;
        }
        sending_ = true;
        boost::asio::spawn(conn_->getWorker()->getIOContext(), [this](boost::asio::yield_context yield) {
            boost::system::error_code ec;
            if (!sending_rtmp_msgs_.empty()) {
                for (auto it = sending_rtmp_msgs_.begin(); it != sending_rtmp_msgs_.end();) {
                    if (!_sendRtmpMessage(*it, yield)) {
                        return;
                    }
                    it = sending_rtmp_msgs_.erase(it);
                }
            }
            sending_ = false;
        });
    };
}

RtmpChunkProtocol::~RtmpChunkProtocol() {

}

bool RtmpChunkProtocol::_sendRtmpMessage(std::shared_ptr<RtmpMessage> rtmp_msg, boost::asio::yield_context & yield) {
    size_t left_size = rtmp_msg->payload_size_;
    size_t cur_pos = 0;
    uint8_t fmt = RTMP_FMT_TYPE0;
    bool first_pkt = true;
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

        if (first_pkt) {
            first_pkt = false;
            fmt = RTMP_FMT_TYPE0;
        } else {
            fmt = RTMP_FMT_TYPE3;
        }

        //  +--------------+----------------+--------------------+--------------+
        //  | Basic Header | Message Header | Extended Timestamp | Chunk Data |
        //  +--------------+----------------+--------------------+--------------+
        // 发送basic header
        std::shared_ptr<RtmpChunk> chunk = std::make_shared<RtmpChunk>();
        chunk->rtmp_message_ = rtmp_msg;
        // Chunk stream IDs 2-63 can be encoded in the 1-byte version of this field.
        if (rtmp_msg->chunk_stream_id_ >= 2 && rtmp_msg->chunk_stream_id_ <= 63) {
            uint8_t d = ((fmt&0x03)<<6) | (rtmp_msg->chunk_stream_id_&0x3f);
            if (!conn_->send(&d, 1, yield)) {
                return false;
            }
        } else if (rtmp_msg->chunk_stream_id_ >= 64 && rtmp_msg->chunk_stream_id_ <= 319) {
            uint8_t buf[2];
            buf[0] = ((fmt&0x03)<<6) | 0x00;
            buf[1] = (rtmp_msg->chunk_stream_id_ - 64) & 0xff;
            if (!conn_->send(buf, 2, yield)) {
                return false;
            }
        } else if (rtmp_msg->chunk_stream_id_ >= 64 && rtmp_msg->chunk_stream_id_ <= 65599) {//这里真是奇葩，64有重叠
            uint8_t buf[3];
            buf[0] = ((fmt&0x03)<<6) | 0x01;
            auto csid = rtmp_msg->chunk_stream_id_ - 64;
            buf[1] = (csid%256) & 0xff;
            buf[2] = (csid/256) & 0xff;
            if (!conn_->send(buf, 3, yield)) {
                return false;
            }
        }
        // 发送message header
        int this_chunk_payload_size = std::min(out_chunk_size_, left_size);
        if (fmt == RTMP_FMT_TYPE0) {
            uint32_t t = htonl(rtmp_msg->timestamp_&0xffffff);
            if (!conn_->send((uint8_t*)&t + 1, 3, yield)) {
                return false;
            }

            t = htonl(rtmp_msg->payload_size_);
            if (!conn_->send((uint8_t*)&t + 1, 3, yield)) {
                return false;
            }
            
            if (!conn_->send(&rtmp_msg->message_type_id_, 1, yield)) {
                return false;
            }

            t = htonl(rtmp_msg->message_stream_id_);
            if (!conn_->send((uint8_t*)&t, 4, yield)) {
                return false;
            }
        } else if (fmt == RTMP_FMT_TYPE1) {
            uint32_t timestamp_delta = rtmp_msg->timestamp_ - prev_chunk->rtmp_message_->timestamp_;
            uint32_t t = htonl(timestamp_delta);
            if (!conn_->send((uint8_t*)&t + 1, 3, yield)) {
                return false;
            }

            t = htonl(rtmp_msg->payload_size_);
            if(!conn_->send((uint8_t*)&t + 1, 3, yield)) {
                return false;
            }
            
            if (!conn_->send(&rtmp_msg->message_type_id_, 1, yield)) {
                return false;
            }
        } else if (fmt == RTMP_FMT_TYPE2) {
            uint32_t timestamp_delta = rtmp_msg->timestamp_ - prev_chunk->rtmp_message_->timestamp_;
            uint32_t t = htonl(timestamp_delta);
            if (!conn_->send((uint8_t*)&t + 1, 3, yield)) {
                return false;
            }
        } else if (fmt == RTMP_FMT_TYPE3) {// no header

        }
        // 发送exttimestamp
        if (rtmp_msg->timestamp_ >= 0x00ffffff) {
            uint32_t t = htonl(rtmp_msg->timestamp_);
            if(!conn_->send((uint8_t*)&t, 4, yield)) {
                return false;
            }
        }
        // 发送chunk data
        if (!conn_->send(rtmp_msg->payload_ + cur_pos, this_chunk_payload_size, yield)) {
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
// 异步方式发送
void RtmpChunkProtocol::sendRtmpMessage(std::shared_ptr<RtmpMessage> rtmp_msg) {
    sending_rtmp_msgs_.emplace_back(rtmp_msg);
    send_handler_();
}

int32_t RtmpChunkProtocol::cycleRecvRtmpMessage(const std::function<bool(std::shared_ptr<RtmpMessage>, boost::asio::yield_context &)> & recv_handler, boost::asio::yield_context & yield) {
    recv_handler_ = recv_handler;

    while(1) {// todo reduce read system call, use recvSome
        // read basic header
        uint8_t d;
        bool ret = conn_->recv(&d, 1, yield);
        if (!ret) {
            return -1;
        }
        
        int32_t cid = (int32_t)(d & 0x3f);
        int8_t fmt = (d >> 6) & 0x03;
        if (cid == 0) {
            if (!conn_->recv(&d, 1, yield)) {
                cid += 64;
                cid += (int32_t)d;
            }
        } else if(cid == 1) {
            uint8_t buf[2];
            if (!conn_->recv(buf, 2, yield)) {
                return -2;
            }
            cid = 64;
            cid += (int32_t)(buf[0]);
            cid += (int32_t)(buf[1])*256;
        }
        // read chunk message header
        std::shared_ptr<RtmpChunk> prev_chunk;
        auto cid_it = recv_chunk_streams_.find(cid);
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
            if (!conn_->recv(t, 3, yield)) {
                return -3;
            }
            uint8_t *p = (uint8_t*)&chunk->chunk_message_header_.timestamp_;
            p[0] = t[2];
            p[1] = t[1];
            p[2] = t[0];

            memset(t, 0, 4);
            if (!conn_->recv(t, 3, yield)) {
                return -4;
            }

            chunk->chunk_message_header_.message_length_ = 0;
            p = (uint8_t*)&chunk->chunk_message_header_.message_length_;
            p[0] = t[2];
            p[1] = t[1];
            p[2] = t[0];

            if (!conn_->recv(&chunk->chunk_message_header_.message_type_id_, 1, yield)) {
                return -5;
            }

            memset(t, 0, 4);
            if(!conn_->recv(t, 4, yield)) {
                return -6;
            }
            p = (uint8_t*)&chunk->chunk_message_header_.message_stream_id_;
            p[0] = t[3];
            p[1] = t[2];
            p[2] = t[1];
            p[3] = t[0];

            if (chunk->chunk_message_header_.timestamp_ == 0x00ffffff) {
                memset(t, 0, 4);
                if(!conn_->recv(t, 4, yield)) {
                    return -7;
                }
                p = (uint8_t*)&chunk->chunk_message_header_.timestamp_;
                p[0] = t[3];
                p[1] = t[2];
                p[2] = t[1];
                p[3] = t[0];
            }
        } else if (fmt == RTMP_FMT_TYPE1) {
            if (!prev_chunk) {//type1 必须有前面的chunk作为基础
                return -8;
            }
            *chunk = *prev_chunk;

            uint8_t t[4] = {0};
            if(!conn_->recv(t, 3, yield)) {
                return -9;
            }

            int32_t time_delta = 0;
            uint8_t *p = (uint8_t *)&time_delta;
            p[0] = t[2];
            p[1] = t[1];
            p[2] = t[0];
            chunk->chunk_message_header_.timestamp_ = prev_chunk->chunk_message_header_.timestamp_ + time_delta;
            
            memset(t, 0, 4);
            if(!conn_->recv(t, 3, yield)) {
                return -10;
            }
            p = (uint8_t *)&chunk->chunk_message_header_.message_length_;
            p[0] = t[2];
            p[1] = t[1];
            p[2] = t[0];

            if(!conn_->recv((uint8_t*)&chunk->chunk_message_header_.message_type_id_, 1, yield)) {
                return -11;
            }
        } else if (fmt == RTMP_FMT_TYPE2) {
            if (!prev_chunk) {//type2 必须有前面的chunk作为基础
                return -12;
            }
            *chunk = *prev_chunk;

            uint8_t t[4] = {0};
            if(!conn_->recv(t, 3, yield)) {
                return -13;
            }
            int32_t time_delta = 0;
            uint8_t *p = (uint8_t *)&time_delta;
            p[0] = t[2];
            p[1] = t[1];
            p[2] = t[0];
            chunk->chunk_message_header_.timestamp_ = prev_chunk->chunk_message_header_.timestamp_ + time_delta;
        } else if (fmt == RTMP_FMT_TYPE3) {
            if (!prev_chunk) {
                return -14;
            }
            *chunk = *prev_chunk;
        }
        recv_chunk_streams_[cid] = chunk;

        if (prev_chunk) {
            prev_chunk->clear();
            recv_chunk_cache_[cid] = prev_chunk;
        }

        if (chunk->chunk_message_header_.message_length_ >= 2*1024*1024) {// packet too big
            return -14;
        }

        if (!chunk->rtmp_message_) {
            chunk->rtmp_message_ = std::make_shared<RtmpMessage>(chunk->chunk_message_header_.message_length_);
        }
        // read the payload
        int32_t this_chunk_payload_size = std::min(in_chunk_size_, chunk->chunk_message_header_.message_length_ - chunk->rtmp_message_->payload_size_);
        if(!conn_->recv(chunk->rtmp_message_->payload_ + chunk->rtmp_message_->payload_size_, this_chunk_payload_size, yield)) {
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
                    return -16;
                }      
                chunk->rtmp_message_.reset(); 
                continue;
            } else if (chunk->rtmp_message_->message_type_id_ == RTMP_MESSAGE_TYPE_ABORT_MESSAGE) {
                if (!handleAbort(chunk->rtmp_message_)) {
                    return -17;
                }
                chunk->rtmp_message_.reset();       
                continue;
            }

            if (!recv_handler_(chunk->rtmp_message_, yield)) {
                return -18;
            }

            chunk->rtmp_message_.reset();
        }
    }
}

size_t RtmpChunkProtocol::getOutChunkSize() {
    return out_chunk_size_;
}

void RtmpChunkProtocol::setOutChunkSize(size_t s) {
    out_chunk_size_ = s;
}

void RtmpChunkProtocol::close() {
    conn_->close();
}

bool RtmpChunkProtocol::handleSetChunkSize(std::shared_ptr<RtmpMessage> msg) {
    RtmpSetChunkSizeMessage cmd;
    int ret = cmd.decode(msg);
    if (ret <= 0) {// 解析成功时，返回解析成功字节数
        return false;
    }
    in_chunk_size_ = cmd.chunk_size_;
    return true;
}

bool RtmpChunkProtocol::handleAbort(std::shared_ptr<RtmpMessage> msg) {
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
