#pragma once
#include <memory>
#include <map>

#include "base/thread/thread_worker.hpp"
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include "core/session.hpp"
#include "core/rtp_media_source.hpp"
#include "core/rtp_media_sink.hpp"
#include "protocol/sdp/sdp.hpp"

#include <boost/asio/ip/udp.hpp>
#include <boost/asio/spawn.hpp>

#include "base/network/udp_socket.hpp"
#include "server/stun/protocol/stun_msg.h"

#include "dtls/dtls_session.h"
#include "srtp/srtp_session.h"
namespace mms {
class WebsocketServer;
class WebSocketConn;
class WebRtcSession  : public Session, public std::enable_shared_from_this<WebRtcSession>, public RtpMediaSource, public RtpMediaSink {
public:
    WebRtcSession(ThreadWorker *worker, WebSocketConn *conn);
    virtual ~WebRtcSession();

    void service();
    void close() override;
    ThreadWorker *getWorker() {
        return worker_;
    }

    const std::string & getLocalICEUfrag() const {
        return local_ice_ufrag_;
    }

    const std::string & getLocalICEPwd() const {
        return local_ice_pwd_;
    }

    bool processStunPacket(std::shared_ptr<StunMsg> stun_msg, std::unique_ptr<uint8_t[]> data, size_t len, UdpSocket *sock, const boost::asio::ip::udp::endpoint &remote_ep);
    void onMessage(websocketpp::server<websocketpp::config::asio>* server, websocketpp::connection_hdl hdl, websocketpp::server<websocketpp::config::asio>::message_ptr msg);

    void setDtlsCert(std::shared_ptr<DtlsCert> dtls_cert);
    bool processDtlsPacket(std::unique_ptr<uint8_t[]> data, size_t len, UdpSocket *sock, const boost::asio::ip::udp::endpoint &remote_ep);
    bool processSRtpPacket(std::unique_ptr<uint8_t[]> data, size_t len, UdpSocket *sock, const boost::asio::ip::udp::endpoint &remote_ep);
private:
    bool processStunBindingReq(std::shared_ptr<StunMsg> stun_msg, UdpSocket *sock, const boost::asio::ip::udp::endpoint &remote_ep, boost::asio::yield_context & yield);
    bool processOfferMsg(websocketpp::server<websocketpp::config::asio>* server, websocketpp::connection_hdl hdl, const std::string & sdp);
    int32_t createLocalSdp();
    int32_t sendLocalSdp(websocketpp::server<websocketpp::config::asio>* server, websocketpp::connection_hdl hdl);
    void onDtlsHandshakeDone(SRTPProtectionProfile profile, const std::string & srtp_recv_key, const std::string & srtp_send_key);
private:
    ThreadWorker *worker_;
    WebSocketConn *ws_conn_;
    Sdp remote_sdp_;
    Sdp local_sdp_;
    uint64_t session_id_ = 0;
    std::string app_;
    std::string stream_;
    std::string local_ice_ufrag_;
    std::string local_ice_pwd_;
    std::string remote_ice_ufrag_;
    std::string remote_ice_pwd_;

    std::shared_ptr<DtlsCert> dtls_cert_;
    DtlsSession dtls_session_;
    SRTPSession srtp_session_;

    uint8_t audio_pt_ = 111;
    uint8_t video_pt_ = 127;

    uint32_t rtcp_pkt_count_ = 0;
};

};