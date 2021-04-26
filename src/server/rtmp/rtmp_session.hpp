#pragma once
#include <memory>

#include "rtmp_conn.hpp"
#include "rtmp_handshake.hpp"
#include "rtmp_protocol.hpp"

namespace mms {
class RtmpSession {
public:
    RtmpSession(RtmpConn *conn):conn_(conn), handshake_(conn) {
    }

    virtual ~RtmpSession() {

    }

    void service();
private:
    bool handleAmf0Command(std::shared_ptr<RtmpChunk> chunk);
    bool handleAmf0ConnectCommand(char *payload, size_t len);

    bool handleSetChunkSize(std::shared_ptr<RtmpChunk> chunk);
    bool handleAbort(std::shared_ptr<RtmpChunk> chunk);
    bool handleAcknowledgement(std::shared_ptr<RtmpChunk> chunk);
    bool handleUserControlMsg(std::shared_ptr<RtmpChunk> chunk);
    
    RtmpConn *conn_;
    RtmpHandshake handshake_;
    int32_t in_chunk_size_ = 128;
    int32_t out_chunk_size_ = 128;
    int32_t window_ack_size_ = 2*1024*1024;
    boost::array<char, 1024*1024> buffer_;
    
    std::unordered_map<uint32_t, std::shared_ptr<RtmpChunk>> chunk_streams_;
};

};