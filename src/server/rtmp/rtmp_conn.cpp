#include <iostream>
#include "rtmp_conn.hpp"

namespace mms {

void RtmpConn::doService() {
    boost::array<char, 1537> c0c1;
    if(!recv(c0c1.data(), 1537)) {
        close(); // 关闭socket
        return;
    }

    boost::array<char, 3073> s0s1s2;
    _genS0S1S2(c0c1.data(), s0s1s2.data());
    // send s0, s1, s2
    if(!send(s0s1s2.data(), 3073)) {
        close(); // 关闭socket
        return;
    }

    boost::array<char, 1536> c2;
    if(!recv(c2.data(), 1536)) {
        close(); // 关闭socket
        return;
    }
    // handshake done
    while(1) {
        // read basic header
        char d;
        bool ret = recv(&d, 1);
        if (!ret) {
            close();
            return;
        }
        
        int32_t cid = (int32_t)(d & 0x3f);
        int8_t fmt = (d >> 6) & 0x03;
        if (cid == 0) {
            if (!recv(&d, 1)) {
                cid += 64;
                cid += (int32_t)d;
            }
        } else if(cid == 1) {
            char buf[2];
            if (!recv(buf, 2)) {
                close();
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
        auto chunk = std::make_shared<RtmpChunk>();
        if (fmt == 0) {
            char t[4] = {0};
            if(!recv(t+1, 3)) {
                close();
                return;
            }
            chunk->chunk_message_header_.timestamp_ = ntohl(*(int32_t*)t);

            memset(t, 0, 4);
            if(!recv(t+1, 3)) {
                close();
                return;
            }
            chunk->chunk_message_header_.message_length_ = ntohl(*(int32_t*)t);

            if(!recv((char*)&chunk->chunk_message_header_.message_type_id_, 1)) {
                close();
                return;
            }

            memset(t, 0, 4);
            if(!recv(t, 4)) {
                close();
                return;
            }
            chunk->chunk_message_header_.message_stream_id_ = ntohl(*(int32_t*)t);

            if (chunk->chunk_message_header_.timestamp_ == 0x00ffffff) {
                memset(t, 0, 4);
                if(!recv(t, 4)) {
                    close();
                    return;
                }
                chunk->chunk_message_header_.timestamp_ = ntohl(*(int32_t*)t);
            }
        } else if (fmt == 1) {
            if (!prev_chunk) {//type1 必须有前面的chunk作为基础
                close();
                return;
            }
            *chunk = *prev_chunk;

            char t[4] = {0};
            if(!recv(t+1, 3)) {
                close();
                return;
            }

            int32_t time_delta = ntohl(*(int32_t*)t);
            chunk->chunk_message_header_.timestamp_ = prev_chunk->chunk_message_header_.timestamp_ + time_delta;
            
            memset(t, 0, 4);
            if(!recv(t+1, 3)) {
                close();
                return;
            }
            chunk->chunk_message_header_.message_length_ = ntohl(*(int32_t*)t);

            if(!recv((char*)&chunk->chunk_message_header_.message_type_id_, 1)) {
                close();
                return;
            }

            chunk->chunk_message_header_.message_type_id_ = prev_chunk->chunk_message_header_.message_type_id_;
        } else if (fmt == 2) {
            if (!prev_chunk) {//type1 必须有前面的chunk作为基础
                close();
                return;
            }
            *chunk = *prev_chunk;

            char t[4] = {0};
            if(!recv(t+1, 3)) {
                close();
                return;
            }
            int32_t time_delta = ntohl(*(int32_t*)t);
            chunk->chunk_message_header_.timestamp_ = prev_chunk->chunk_message_header_.timestamp_ + time_delta;
        } else if (fmt == 3) {
            *chunk = *prev_chunk;
        }
        chunk_streams_[cid] = chunk;

        if (chunk->chunk_message_header_.message_length_ >= 2*1024*1024) {// packet too big
            close();
            return;
        }   

        if (!chunk->rtmp_message_) {
            chunk->rtmp_message_ = new RtmpMessage(chunk->chunk_message_header_.message_length_);
        }
        // read the payload
        int32_t this_chunk_payload_size = std::min(in_chunk_size_, chunk->chunk_message_header_.message_length_ - chunk->rtmp_message_->curr_size_);
        if(!recv(chunk->rtmp_message_->payload_ + chunk->rtmp_message_->curr_size_, this_chunk_payload_size)) {
            close();
            return;
        }
        in_chunk_size_ = 4096;
        chunk->rtmp_message_->curr_size_ += this_chunk_payload_size;
        // if we get a rtmp message
        if (chunk->rtmp_message_->curr_size_ == chunk->chunk_message_header_.message_length_) {
            // process this chunk->rtmp_message_;
            delete chunk->rtmp_message_;
            chunk->rtmp_message_ = nullptr;
            // release this chunk->rtmp_message_
        }
    }
}

void RtmpConn::_genS0S1S2(char *c0c1, char *s0s1s2) {
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