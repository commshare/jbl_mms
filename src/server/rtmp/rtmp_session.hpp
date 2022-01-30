#pragma once
#include <memory>
#include "base/shared_ptr.hpp"
#include "server/rtmp/rtmp_conn/rtmp_conn.hpp"
#include "server/rtmp/rtmp_protocol/rtmp_handshake.hpp"
#include "server/rtmp/rtmp_protocol/rtmp_chunk_protocol.hpp"
#include "core/media_stream.hpp"
#include "core/media_source.hpp"
#include "core/media_sink.hpp"
#include "core/rtmp_media_source.hpp"
#include "core/session.hpp"

namespace mms {
class RtmpSession : public Session {
public:
    RtmpSession(RtmpConn *conn);

    virtual ~RtmpSession() {

    }

    void service();
    void close();
private:
    bool onRecvRtmpMessage(std::shared_ptr<RtmpMessage> msg);

    bool handleAmf0Command(std::shared_ptr<RtmpMessage> msg);
    bool handleAmf0ConnectCommand(std::shared_ptr<RtmpMessage> msg);
    bool handleAmf0ReleaseStreamCommand(std::shared_ptr<RtmpMessage> rtmp_msg);
    bool handleAmf0FCPublishCommand(std::shared_ptr<RtmpMessage> rtmp_msg);
    bool handleAmf0CreateStreamCommand(std::shared_ptr<RtmpMessage> rtmp_msg);
    bool handleAmf0PublishCommand(std::shared_ptr<RtmpMessage> rtmp_msg);
    bool handleVideoMsg(std::shared_ptr<RtmpMessage> msg);
    bool handleAudioMsg(std::shared_ptr<RtmpMessage> msg);

    bool handleAmf0Data(std::shared_ptr<RtmpMessage> rtmp_msg);

    bool handleAcknowledgement(std::shared_ptr<RtmpMessage> msg);
    bool handleUserControlMsg(std::shared_ptr<RtmpMessage> msg);
    
    RtmpConn *conn_;
    RtmpHandshake handshake_;
    RtmpChunkProtocol chunk_protocol_;
    uint32_t window_ack_size_ = 80000000;
private:
    std::shared_ptr<RtmpMediaSource> media_source_ = nullptr;
};

};