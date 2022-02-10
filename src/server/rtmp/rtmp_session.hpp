#pragma once
#include <memory>
#include "base/shared_ptr.hpp"
#include "server/rtmp/rtmp_conn/rtmp_conn.hpp"
#include "server/rtmp/rtmp_protocol/rtmp_handshake.hpp"
#include "server/rtmp/rtmp_protocol/rtmp_chunk_protocol.hpp"

#include "rtmp_message/command_message/rtmp_connect_command_message.hpp"
#include "rtmp_message/command_message/rtmp_publish_message.hpp"
#include "rtmp_message/command_message/rtmp_play_message.hpp"

#include "core/rtmp_media_source.hpp"
#include "core/rtmp_media_sink.hpp"
#include "core/session.hpp"
#include "boost/coroutine2/all.hpp"

namespace mms {
class RtmpSession : public Session, public RtmpMediaSource, public RtmpMediaSink, public std::enable_shared_from_this<RtmpSession> {
public:
    RtmpSession(RtmpConn *conn);

    virtual ~RtmpSession() {

    }

    void service();
    void close();
private:
    bool onRecvRtmpMessage(std::shared_ptr<RtmpMessage> msg, boost::asio::yield_context & yield);
    bool sendRtmpMessage(std::shared_ptr<RtmpMessage> msg);

    bool handleAmf0Command(std::shared_ptr<RtmpMessage> msg, boost::asio::yield_context & yield);
    bool handleAmf0ConnectCommand(std::shared_ptr<RtmpMessage> msg, boost::asio::yield_context & yield);
    bool handleAmf0ReleaseStreamCommand(std::shared_ptr<RtmpMessage> rtmp_msg, boost::asio::yield_context & yield);
    bool handleAmf0FCPublishCommand(std::shared_ptr<RtmpMessage> rtmp_msg, boost::asio::yield_context & yield);
    bool handleAmf0CreateStreamCommand(std::shared_ptr<RtmpMessage> rtmp_msg, boost::asio::yield_context & yield);
    bool handleAmf0PublishCommand(std::shared_ptr<RtmpMessage> rtmp_msg, boost::asio::yield_context & yield);
    bool handleAmf0PlayCommand(std::shared_ptr<RtmpMessage> rtmp_msg, boost::asio::yield_context & yield);
    bool handleVideoMsg(std::shared_ptr<RtmpMessage> msg, boost::asio::yield_context & yield);
    bool handleAudioMsg(std::shared_ptr<RtmpMessage> msg, boost::asio::yield_context & yield);

    bool handleAmf0Data(std::shared_ptr<RtmpMessage> rtmp_msg, boost::asio::yield_context & yield);

    bool handleAcknowledgement(std::shared_ptr<RtmpMessage> msg, boost::asio::yield_context & yield);
    bool handleUserControlMsg(std::shared_ptr<RtmpMessage> msg, boost::asio::yield_context & yield);

    RtmpConn *conn_;
    RtmpHandshake handshake_;
    RtmpChunkProtocol chunk_protocol_;
    uint32_t window_ack_size_ = 80000000;
private:
    bool parseConnectCmd(RtmpConnectCommandMessage & conn_cmd);
    bool parsePublishCmd(RtmpPublishMessage & pub_cmd);
    bool parsePlayCmd(RtmpPlayMessage & pub_cmd);
    std::function<void()> send_handler_;
private:
    ThreadWorker *worker_;
    std::string domain_;
    std::string app_;
    std::string stream_name_;
    std::string session_name_;
};

};