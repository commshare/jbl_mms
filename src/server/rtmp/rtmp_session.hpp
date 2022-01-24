#pragma once
#include <memory>

#include "server/rtmp/rtmp_conn/rtmp_conn.hpp"
#include "server/rtmp/rtmp_protocol/rtmp_handshake.hpp"
#include "server/rtmp/rtmp_protocol/rtmp_chunk_protocol.hpp"

namespace mms {
class RtmpSession {
public:
    RtmpSession(RtmpConn *conn);

    virtual ~RtmpSession() {

    }

    void service();
private:
    int32_t onRecvRtmpMessage(std::shared_ptr<RtmpMessage> msg);

    bool handleAmf0Command(std::shared_ptr<RtmpMessage> msg);
    bool handleAmf0ConnectCommand(std::shared_ptr<RtmpMessage> msg);
    bool handleAcknowledgement(std::shared_ptr<RtmpMessage> msg);
    bool handleUserControlMsg(std::shared_ptr<RtmpMessage> msg);
    
    RtmpConn *conn_;
    RtmpHandshake handshake_;
    RtmpChunkProtocol chunk_protocol_;
    uint32_t window_ack_size_ = 80000000;
};

};