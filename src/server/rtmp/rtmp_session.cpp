#include <iostream>
#include "rtmp_define.hpp"
#include "rtmp_session.hpp"

#include "./amf0/amf0_string.hpp"
#include "./amf0/amf0_number.hpp"
#include "./amf0/amf0_object.hpp"

#include "rtmp_command_message.hpp"
#include "rtmp_window_ack_size_message.hpp"

namespace mms {
RtmpSession::RtmpSession(RtmpConn *conn):conn_(conn), handshake_(conn) {
    for (uint32_t cid = 0; cid < 256; cid++) {
        chunk_cache_[cid] = std::make_shared<RtmpChunk>();
    }
}

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
            chunk->rtmp_message_ = new RtmpMessage(chunk->chunk_message_header_.message_length_);
        }
        // read the payload
        int32_t this_chunk_payload_size = std::min(in_chunk_size_, chunk->chunk_message_header_.message_length_ - chunk->rtmp_message_->curr_size_);
        if(!conn_->recv(chunk->rtmp_message_->payload_ + chunk->rtmp_message_->curr_size_, this_chunk_payload_size)) {
            conn_->close();
            return;
        }

        chunk->rtmp_message_->curr_size_ += this_chunk_payload_size;
        // if we get a rtmp message
        if (chunk->rtmp_message_->curr_size_ == chunk->chunk_message_header_.message_length_) {
            // process this chunk->rtmp_message_;
            std::cout << "got a rtmp message" << std::endl;
            if (!handleRtmpMessage(chunk)) {
                conn_->close();
            }

            delete chunk->rtmp_message_;
            chunk->rtmp_message_ = nullptr;
            // release this chunk->rtmp_message_
        }
    }
}

bool RtmpSession::handleRtmpMessage(std::shared_ptr<RtmpChunk> chunk) {
    switch(chunk->chunk_message_header_.message_type_id_) {
        case RTMP_MESSAGE_SET_CHUNK_SIZE: {
            return handleSetChunkSize(chunk);
        }
        case RTMP_MESSAGE_ABORT_MSG: {
            return handleAbort(chunk);
        }
        case RTMP_MESSAGE_USER_CONTROL: {
            return handleUserControlMsg(chunk);
        }
        case RTMP_MESSAGE_ACKNOWLEDGEMENT: {
            return handleAcknowledgement(chunk);
        }
        case RTMP_MESSAGE_AMF0_COMMAND: {
            return handleAmf0Command(chunk);
        }
        default : {
            return false;
        }
    }
    return false;
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
    RtmpConnectCommandMessage connect_command;
    auto consumed = connect_command.decode(payload, len);
    if(consumed < 0) {
        return false;
    }
    payload += consumed;
    len -= consumed;
    // send window ack size to client
    RtmpWindowAckSizeMessage ack(window_ack_size_);
    // std::vector<boost::shared_ptr<RtmpChunk>> chunks = ack.toChunk(out_chunk_size_);

    return true;
}

bool RtmpSession::handleSetChunkSize(std::shared_ptr<RtmpChunk> chunk) {
    char * payload = chunk->rtmp_message_->payload_;
    int32_t len = chunk->rtmp_message_->curr_size_;
    if (len < 4) {
        return false;
    }

    int32_t s = 0;
    char *p = (char*)&s;
    p[0] = payload[3];
    p[1] = payload[2];
    p[2] = payload[1];
    p[3] = payload[0];
    in_chunk_size_ = s;
    return true;
}

bool RtmpSession::handleAbort(std::shared_ptr<RtmpChunk> chunk) {
    char * payload = chunk->rtmp_message_->payload_;
    int32_t len = chunk->rtmp_message_->curr_size_;
    if (len < 4) {
        return false;
    }

    uint32_t chunk_id = 0;
    char *p = (char*)&chunk_id;
    p[0] = payload[3];
    p[1] = payload[2];
    p[2] = payload[1];
    p[3] = payload[0];

    auto it = chunk_streams_.find(chunk_id);
    if (it != chunk_streams_.end()) {
        chunk_streams_.erase(it);
    }
    return true;
}

bool RtmpSession::handleAcknowledgement(std::shared_ptr<RtmpChunk> chunk) {
    // todo 
    // nothing to do
    return true;
}

bool RtmpSession::handleUserControlMsg(std::shared_ptr<RtmpChunk> chunk) {
    return true;
}

};