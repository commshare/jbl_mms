#include <iostream>

#include "webrtc_session.hpp"
#include "websocket_conn.hpp"
#include "websocket_server.hpp"
#include "json/json.h"
#include "base/utils/utils.h"
#include "config/config.h"

#include "server/stun/protocol/stun_binding_response_msg.hpp"
#include "server/stun/protocol/stun_mapped_address_attr.h"
#include "dtls/dtls_cert.h"

#include "protocol/rtp/rtp_packet.h"

#include "protocol/sdp/attribute/common/dir.hpp"
#include "core/media_manager.hpp"

using namespace mms;
WebRtcSession::WebRtcSession(ThreadWorker *worker, WebSocketConn *conn) : RtpMediaSource(worker), worker_(worker), ws_conn_(conn)
{
    std::cout << "create webrtcsession" << std::endl;
    local_ice_ufrag_ = Utils::randStr(4);
    local_ice_pwd_ = Utils::randStr(24);
    dtls_session_.init();
    dtls_session_.onHandshakeDone(std::bind(&WebRtcSession::onDtlsHandshakeDone, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

WebRtcSession::~WebRtcSession()
{
    std::cout << "destroy webrtcsession" << std::endl;
}

void WebRtcSession::onMessage(websocketpp::server<websocketpp::config::asio> *server, websocketpp::connection_hdl hdl, websocketpp::server<websocketpp::config::asio>::message_ptr msg)
{
    if (msg->get_opcode() == websocketpp::frame::opcode::text)
    {
        Json::Value root;
        Json::Reader reader;
        if (!reader.parse(msg->get_payload(), root, false))
        {
            std::cout << "not json msg" << std::endl;
            return;
        }

        if (!root.isMember("app") || !root["app"].isString())
        {
            return;
        }

        if (!root.isMember("stream") || !root["stream"].isString())
        {
            return;
        }

        if (!root.isMember("message") || !root["message"].isObject())
        {
            std::cout << "msg is not object" << std::endl;
            return;
        }

        Json::Value &msg = root["message"];
        if (!msg.isMember("type") || !msg["type"].isString())
        {
            std::cout << "msg type is not string" << std::endl;
            return;
        }

        const std::string &type = msg["type"].asString();
        if ("offer" == type)
        {
            if (!msg.isMember("sdp") || !msg["sdp"].isString())
            {
                std::cout << "no sdp info" << std::endl;
                return;
            }
            std::string domain = ws_conn_->getDomain();
            const std::string &app = root["app"].asString();
            const std::string &stream = root["stream"].asString();
            
            setSessionName(domain + "/" + app + "/" + stream);// process offer时，要用到session name，所以先设置
            if (!processOfferMsg(server, hdl, msg["sdp"].asString()))
            {
                close();
                return;
            }
            
            auto medias = remote_sdp_.getMediaSdps();
            if (medias.size() < 0)
            {
                close();
                return;
            }

            if (medias[0].getDir().getDir() == DirAttr::MEDIA_SENDONLY || medias[0].getDir().getDir() == DirAttr::MEDIA_SENDRECV)
            {
                MediaManager::get_mutable_instance().addSource(session_name_, std::dynamic_pointer_cast<MediaSource>(shared_from_this()));
            }
        }
    }
}

bool WebRtcSession::processOfferMsg(websocketpp::server<websocketpp::config::asio> *server, websocketpp::connection_hdl hdl, const std::string &sdp)
{
    session_id_ = Utils::rand64();
    auto ret = remote_sdp_.parse(sdp);
    if (0 != ret)
    {
        return false;
    }

    auto remote_ice_ufrag = remote_sdp_.getIceUfrag();
    if (!remote_ice_ufrag)
    {
        return false;
    }
    remote_ice_ufrag_ = remote_ice_ufrag.value().getUfrag();

    auto remote_ice_pwd = remote_sdp_.getIcePwd();
    if (!remote_ice_pwd)
    {
        return false;
    }
    remote_ice_pwd_ = remote_ice_pwd.value().getPwd();

    if (0 != createLocalSdp())
    {
        return false;
    }

    if (0 != sendLocalSdp(server, hdl))
    {
        return false;
    }
    return true;
}

int32_t WebRtcSession::createLocalSdp()
{
    local_sdp_.setVersion(0);
    local_sdp_.setOrigin({"-", Utils::rand64(), 1, "IN", "IP4", "127.0.0.1"}); // o=- rand64 1 IN IP4 127.0.0.1
    local_sdp_.setSessionName(session_name_);                                  //
    local_sdp_.setTime({0, 0});                                                // t=0 0
    local_sdp_.setTool({"mms"});
    local_sdp_.setBundle({"video", "audio", "data"});
    local_sdp_.addAttr("ice-lite");
    local_sdp_.addAttr("msid-semantic: WMS " + session_name_);
    auto &remote_medias = remote_sdp_.getMediaSdps();
    if (remote_medias.size() > 1)
    {
        BundleAttr bundle;
        for (auto &media : remote_medias)
        {
            if (media.getMedia() == "audio")
            {
                bundle.addMid(media.getMidAttr().getMid());
            }
            else if (media.getMedia() == "video")
            {
                bundle.addMid(media.getMidAttr().getMid());
            }
        }
        local_sdp_.setBundle(bundle);
    }

    for (auto &media : remote_medias)
    {
        if (media.getMedia() == "audio")
        {
            MediaSdp audio_sdp;
            audio_sdp.setMedia("audio");
            audio_sdp.setPort(9);
            audio_sdp.setPortCount(1);
            audio_sdp.setProto("UDP/TLS/RTP/SAVPF");
            audio_sdp.addFmt(96);
            audio_sdp.setConnectionInfo({"IN", "IP4", "0.0.0.0"});
            audio_sdp.setIceUfrag(IceUfrag(local_ice_ufrag_));
            audio_sdp.setIcePwd(IcePwd(local_ice_pwd_));
            audio_sdp.setDir(media.getReverseDir());
            audio_sdp.setSetup(SetupAttr(ROLE_PASSIVE));
            audio_sdp.setMidAttr(media.getMidAttr());
            audio_sdp.setRtcpMux(RtcpMux());
            audio_sdp.addCandidate(Candidate("fund_common", 1, "UDP", 2130706431, ws_conn_->getLocalIp(), Config::getInstance().getWebrtcUdpPort(), Candidate::CAND_TYPE_HOST, "", 0, {{"generation", "0"}}));
            if (media.getSsrcGroup()) 
            {
                audio_sdp.setSsrcGroup(media.getSsrcGroup().value());
            }

            for (auto & p : media.getSsrcs()) {
                audio_sdp.addSsrc(Ssrc(p.second.getId(), session_name_, session_name_, session_name_ + "_audio"));
            }
            
            audio_sdp.setFingerPrint(FingerPrint("sha-1", dtls_cert_->getFingerPrint()));
            auto remote_audio_payload = media.searchPayload("opus");
            if (!remote_audio_payload.has_value())
            {
                return -12;
            }

            auto &rap = remote_audio_payload.value();
            Payload audio_payload(96, rap.getEncodingName(), rap.getClockRate(), rap.getEncodingParams());
            audio_payload.addRtcpFb(RtcpFb(96, "nack"));
            audio_payload.addRtcpFb(RtcpFb(96, "nack", "pli"));
            audio_sdp.addPayload(audio_payload);

            local_sdp_.addMediaSdp(audio_sdp);
        }
        else if (media.getMedia() == "video")
        {
            MediaSdp video_sdp;
            video_sdp.setMedia("video");
            video_sdp.setPort(9);
            video_sdp.setPortCount(1);
            video_sdp.setProto("UDP/TLS/RTP/SAVPF");
            video_sdp.addFmt(97);
            video_sdp.setConnectionInfo({"IN", "IP4", "0.0.0.0"});
            video_sdp.setIceUfrag(IceUfrag(local_ice_ufrag_));
            video_sdp.setIcePwd(IcePwd(local_ice_pwd_));
            video_sdp.setDir(media.getReverseDir());
            video_sdp.setSetup(SetupAttr(ROLE_PASSIVE));
            video_sdp.setMidAttr(media.getMidAttr());
            video_sdp.addCandidate(Candidate("fund_common", 1, "UDP", 2130706431, ws_conn_->getLocalIp(), Config::getInstance().getWebrtcUdpPort(), Candidate::CAND_TYPE_HOST, "", 0, {{"generation", "0"}}));
            video_sdp.setRtcpMux(RtcpMux());
            if (media.getSsrcGroup()) 
            {
                video_sdp.setSsrcGroup(media.getSsrcGroup().value());
            }

            for (auto & p : media.getSsrcs()) {
                video_sdp.addSsrc(Ssrc(p.second.getId(), session_name_, session_name_, session_name_ + "_video"));
            }

            video_sdp.setFingerPrint(FingerPrint("sha-1", dtls_cert_->getFingerPrint()));
            auto remote_video_payload = media.searchPayload("H264");
            if (!remote_video_payload.has_value())
            {
                return -13;
            }
            auto &rvp = remote_video_payload.value();
            Payload video_payload(97, rvp.getEncodingName(), rvp.getClockRate(), rvp.getEncodingParams());
            video_payload.addRtcpFb(RtcpFb(97, "ccm", "fir"));
            video_payload.addRtcpFb(RtcpFb(97, "goog-remb"));
            video_payload.addRtcpFb(RtcpFb(97, "nack"));
            video_payload.addRtcpFb(RtcpFb(97, "nack", "pli"));
            video_payload.addRtcpFb(RtcpFb(97, "transport-cc"));

            video_sdp.addPayload(video_payload);
            local_sdp_.addMediaSdp(video_sdp);
        }
    }
    return 0;
}

int32_t WebRtcSession::sendLocalSdp(websocketpp::server<websocketpp::config::asio>* server, websocketpp::connection_hdl hdl)
{
    std::string sdp = local_sdp_.toString();
    Json::Value root;
    Json::Value message;
    message["type"] = "answer";
    message["sdp"] = sdp;
    root["message"] = message;

    try
    {
        server->send(hdl, root.toStyledString(), websocketpp::frame::opcode::text);
    }
    catch (websocketpp::exception const &e)
    {
        std::cout << "Echo failed because: "
                  << "(" << e.what() << ")" << std::endl;
    }

    std::cout << "local sdp:" << sdp << std::endl;
    return 0;
}

bool WebRtcSession::processStunPacket(StunMsg &stun_msg, uint8_t *data, size_t len, UdpSocket *sock, const boost::asio::ip::udp::endpoint &remote_ep, boost::asio::yield_context &yield)
{
    const std::string &pwd = getLocalICEPwd();
    if (!stun_msg.checkMsgIntegrity(data, len, pwd))
    {
        std::cout << "check msg integrity failed." << std::endl;
        return false;
    }

    if (!stun_msg.checkFingerPrint(data, len))
    {
        std::cout << "check finger print failed." << std::endl;
        return false;
    }

    switch (stun_msg.type())
    {
    case STUN_BINDING_REQUEST:
    {
        // 返回响应
        return processStunBindingReq(stun_msg, sock, remote_ep, yield);
    }
    }
    return true;
}

bool WebRtcSession::processStunBindingReq(StunMsg &stun_msg, UdpSocket *sock, const boost::asio::ip::udp::endpoint &remote_ep, boost::asio::yield_context &yield)
{
    StunBindingResponseMsg binding_resp(stun_msg);

    auto mapped_addr_attr = std::make_unique<StunMappedAddressAttr>(remote_ep.address().to_v4().to_uint(), remote_ep.port());
    binding_resp.addAttr(std::move(mapped_addr_attr));

    // 校验完整性
    auto req_username_attr = stun_msg.getUserNameAttr();
    if (!req_username_attr)
    {
        return false;
    }

    const std::string &local_user_name = req_username_attr.value().getLocalUserName();
    if (local_user_name.empty())
    {
        return false;
    }

    const std::string &remote_user_name = req_username_attr.value().getRemoteUserName();
    if (remote_user_name.empty())
    {
        return false;
    }

    if (remote_user_name != remote_ice_ufrag_)//用户名与sdp中给的不一致
    {
        return false;
    }

    StunUsernameAttr resp_username_attr(local_user_name, remote_user_name);
    binding_resp.setUserNameAttr(resp_username_attr);
    auto size = binding_resp.size(true, true);
    std::unique_ptr<uint8_t[]> data = std::unique_ptr<uint8_t[]>(new uint8_t[size]);
    int32_t consumed = binding_resp.encode(data.get(), size, true, local_ice_pwd_, true);
    if (consumed < 0) 
    {// todo:add log
        return false;
    }

    // 自己校验下
    // StunMsg stun_msg_check;
    // int32_t ret = stun_msg_check.decode(data.get(), size);
    // if (0 != ret) 
    // {
    //     std::cout << "********************* stun_msg_check failed *********************" << std::endl;
    // }
    // else
    // {
    //     if (!stun_msg_check.checkMsgIntegrity(data.get(), size, local_ice_pwd_))
    //     {
    //         std::cout << "check msg integrity failed." << std::endl;
    //         return false;
    //     }

    //     if (!stun_msg_check.checkFingerPrint(data.get(), size))
    //     {
    //         std::cout << "check finger print failed." << std::endl;
    //         return false;
    //     }
    // }

    if (!sock->sendTo(std::move(data), size, remote_ep, yield)) 
    {//todo log error
        return false; 
    }

    
    return true;
}

bool WebRtcSession::processDtlsPacket(uint8_t *data, size_t len, UdpSocket *sock, const boost::asio::ip::udp::endpoint &remote_ep, boost::asio::yield_context & yield)
{
    bool ret = dtls_session_.processDtlsPacket(data, len, sock, remote_ep, yield);
    if (!ret) {
        std::cout << "process dtls packet failed" << std::endl;
        return false;
    }
    std::cout << "process dtls packet success" << std::endl;
    return true;
}

void WebRtcSession::setDtlsCert(std::shared_ptr<DtlsCert> dtls_cert)
{
    dtls_cert_ = dtls_cert;
    dtls_session_.setDtlsCert(dtls_cert);
}

void WebRtcSession::onDtlsHandshakeDone(SRTPProtectionProfile profile, const std::string & srtp_recv_key, const std::string & srtp_send_key)
{
    if (!srtp_session_.init(profile, srtp_recv_key, srtp_send_key))
    {
        std::cout << "************************* srtp session init failed ***********************" << std::endl;
    }
}

bool WebRtcSession::processSRtpPacket(uint8_t *data, size_t len, UdpSocket *sock, const boost::asio::ip::udp::endpoint &remote_ep, boost::asio::yield_context & yield)
{
    int out_len = 0;
    if (RtpHeader::isRtcpPacket((const char*)data, len)) 
    {
        out_len = srtp_session_.unprotectSRTCP(data, len);
    }
    else
    {
        out_len = srtp_session_.unprotectSRTP(data, len);
        std::shared_ptr<RtpPacket> rtp_pkt = std::make_shared<RtpPacket>();
        int32_t consumed = rtp_pkt->decode(data, out_len);
        std::cout << "pt:" << (uint32_t)rtp_pkt->header_.pt << ", ssrc:" << rtp_pkt->header_.ssrc << ", seq:" << rtp_pkt->header_.seqnum << std::endl;
    }
    return true;
}

void WebRtcSession::service()
{
}

void WebRtcSession::close()
{
    // if (ws_conn_) {
    //     ws_conn_->close();
    // }
}
