#include <iostream>
#include "rtmp_define.hpp"
#include "rtmp_session.hpp"
#include "./amf0/amf0_object.hpp"

namespace mms {
void RtmpSession::service() {
    if (!handshake_.handshake()) {
        conn_->close(); // 关闭socket
        return;
    }
    // handshake done
    while(1) {// todo reduce read system call
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
        auto chunk = std::make_shared<RtmpChunk>();
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
        } else if (fmt == 3) {
            *chunk = *prev_chunk;
        }
        chunk_streams_[cid] = chunk;

        if (chunk->chunk_message_header_.message_length_ >= 2*1024*1024) {// packet too big
            conn_->close();
            return;
        }   

        if (!chunk->rtmp_message_) {
            chunk->rtmp_message_ = new RtmpMessage(chunk->chunk_message_header_.message_length_);
        }
        // read the payload
        int32_t this_chunk_payload_size = std::min(in_chunk_size_, chunk->chunk_message_header_.message_length_ - chunk->rtmp_message_->curr_size_);
        if(!conn_->recv(chunk->rtmp_message_->payload_ + chunk->rtmp_message_->curr_size_, this_chunk_payload_size)) {
            conn_->close();
            return;
        }
        in_chunk_size_ = 4096;
        chunk->rtmp_message_->curr_size_ += this_chunk_payload_size;
        // if we get a rtmp message
        if (chunk->rtmp_message_->curr_size_ == chunk->chunk_message_header_.message_length_) {
            // process this chunk->rtmp_message_;
            std::cout << "got a rtmp message" << std::endl;
            if (chunk->chunk_message_header_.message_type_id_ == RTMP_MESSAGE_AMF0_COMMAND) {
                std::cout << "amf0 message" << std::endl;
                handleAmf0Command(chunk);
            }

            delete chunk->rtmp_message_;
            chunk->rtmp_message_ = nullptr;
            // release this chunk->rtmp_message_
        }
    }
}


bool RtmpSession::handleAmf0Command(std::shared_ptr<RtmpChunk> chunk) {
    Amf0String command;
    char * payload = chunk->rtmp_message_->payload_;
    int32_t len = chunk->rtmp_message_->curr_size_;

    int32_t consumed = command.decode(payload, len);
    if (consumed < 0) {
        return false;
    }

    payload += consumed;
    len -= consumed;

    auto command_name = command.getValue();
    if (command_name == "connect") {
        handleAmf0ConnectCommand(payload, len);
    }

    return true;
}

bool RtmpSession::handleAmf0ConnectCommand(char *payload, size_t len) {
    Amf0Number transaction;
    auto consumed = transaction.decode(payload, len);
    if(consumed < 0) {
        std::cout << "transaction decode failed, ret:" << consumed << std::endl;
        return false;
    }
    payload += consumed;
    len -= consumed;
    auto transaction_id = transaction.getValue();
    std::cout << "transaction_id:" << transaction_id << std::endl;
    return true;
}

};