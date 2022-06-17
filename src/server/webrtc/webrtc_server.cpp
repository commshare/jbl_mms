#include "webrtc_server.hpp"
#include "websocket_conn.hpp"
#include "webrtc_session.hpp"
#include "config/config.h"

#include "server/stun/protocol/stun_binding_response_msg.hpp"
#include "server/stun/protocol/stun_mapped_address_attr.h"
#include "udp_msg_demultiplex.h"

using namespace mms;

bool WebRtcServer::start()
{
    bool ret = initCerts();
    if (!ret)
    {
        return false;
    }

    ret = WebsocketServer::start();
    if (!ret)
    {
        return false;
    }

    ret = UdpServer::startListen(Config::getInstance().getWebrtcUdpPort());
    if (!ret)
    {
        return false;
    }
    return true;
}

bool WebRtcServer::initCerts()
{
    std::string domain = "mms.cn";
    default_dtls_cert_ = std::make_shared<DtlsCert>();
    if (!default_dtls_cert_->init(domain)) 
    {
        return false;
    }
    return true;
}

void WebRtcServer::onUdpSocketRecv(UdpSocket *sock, std::unique_ptr<uint8_t[]> data, size_t len, boost::asio::ip::udp::endpoint &remote_ep)
{
    auto worker = thread_pool_inst::get_mutable_instance().getWorker(-1);
    boost::asio::spawn(worker->getIOContext(), [this, sock, recv_data = std::move(data), len, remote_ep](boost::asio::yield_context yield) {
        uint8_t *data = recv_data.get();
        UDP_MSG_TYPE t = detectMsgType(data, len);
        if (UDP_MSG_STUN == t) {
            StunMsg stun_msg;
            int32_t ret = stun_msg.decode(data, len);
            if (0 == ret) 
            {
                if (processStunPacket(stun_msg, data, len, sock, remote_ep, yield)) 
                {
                    return;
                }
            }
        } else if (UDP_MSG_DTLS == t) {
            if (!processDtlsPacket(data, len, sock, remote_ep, yield))
            {
                return;
            }
        }
    });
}

bool WebRtcServer::processStunPacket(StunMsg &stun_msg, uint8_t *data, size_t len, UdpSocket *sock, const boost::asio::ip::udp::endpoint &remote_ep, boost::asio::yield_context & yield)
{
    std::cout << "stun_msg.type()=" << (uint32_t)stun_msg.type() << std::endl;
    // 校验完整性
    auto username_attr = stun_msg.getUserNameAttr();
    if (!username_attr)
    {
        return false;
    }

    const std::string &local_user_name = username_attr.value().getLocalUserName();
    if (local_user_name.empty())
    {
        return false;
    }
    std::cout << "local_user_name:" << local_user_name << std::endl;
    std::shared_ptr<WebRtcSession> session;
    {
        std::lock_guard<std::mutex> lck(session_map_mtx_);
        auto it_session = ufrag_session_map_.find(local_user_name);
        if (it_session == ufrag_session_map_.end())
        {
            return false;
        }
        session = it_session->second;
        endpoint_session_map_.insert(std::pair(remote_ep, session));
    }

    if (!session) //todo add log
    {
        return false;
    }
    return session->processStunPacket(stun_msg, data, len, sock, remote_ep, yield);
}

bool WebRtcServer::processDtlsPacket(uint8_t *data, size_t len, UdpSocket *sock, const boost::asio::ip::udp::endpoint &remote_ep, boost::asio::yield_context & yield)
{
    std::shared_ptr<WebRtcSession> session;
    {
        std::lock_guard<std::mutex> lck(session_map_mtx_);
        auto it_session = endpoint_session_map_.find(remote_ep);
        if (it_session == endpoint_session_map_.end())
        {
            return false;
        }
        session = it_session->second;
    }

    if (!session) //todo add log
    {
        return false;
    }

    return session->processDtlsPacket(data, len, sock, remote_ep, yield);
}

void WebRtcServer::onWebsocketOpen(websocketpp::connection_hdl hdl)
{
    try
    {
        websocketpp::server<websocketpp::config::asio>::connection_ptr conn = get_con_from_hdl(hdl);
        
        std::shared_ptr<WebSocketConn> ws_conn = std::make_shared<WebSocketConn>(worker_, conn);
        auto webrtc_session = ws_conn->createSession();
        {
            std::lock_guard<std::mutex> lck(mtx_);
            conn_map_.insert(std::pair(conn, ws_conn));
        }
        {
            std::lock_guard<std::mutex> lck(session_map_mtx_);
            ufrag_session_map_.insert(std::pair(webrtc_session->getLocalICEUfrag(), webrtc_session));
        }

        conn->set_message_handler(std::bind(&WebRtcSession::onMessage, webrtc_session.get(), this, websocketpp::lib::placeholders::_1, websocketpp::lib::placeholders::_2));

        webrtc_session->setDtlsCert(default_dtls_cert_);
        webrtc_session->service();
    }
    catch (std::exception &e)
    {
        std::cout << "open websocket failed." << std::endl;
    }
}

void WebRtcServer::onWebsocketClose(websocketpp::connection_hdl hdl)
{
    websocketpp::server<websocketpp::config::asio>::connection_ptr conn = get_con_from_hdl(hdl);
    std::shared_ptr<WebSocketConn> ws_conn;
    {
        std::lock_guard<std::mutex> lck(mtx_);
        auto it = conn_map_.find(conn);
        if (it == conn_map_.end())
        {
            return;
        }
        ws_conn = it->second;
    }

    conn_map_.erase(conn);
}

// // @https://tools.ietf.org/html/draft-ietf-avtcore-rfc5764-mux-fixes
// // NEW TEXT
// //    The process for demultiplexing a packet is as follows.  The receiver
// //    looks at the first byte of the packet.  If the value of this byte is
// //    in between 0 and 3 (inclusive), then the packet is STUN.  If the
// //    value is between 16 and 19 (inclusive), then the packet is ZRTP.  If
// //    the value is between 20 and 63 (inclusive), then the packet is DTLS.
// //    If the value is between 64 and 79 (inclusive), then the packet is
// //    TURN Channel.  If the value is in between 128 and 191 (inclusive),
// //    then the packet is RTP (or RTCP, if both RTCP and RTP are being
// //    multiplexed over the same destination port).  If the value does not
// //    match any known range then the packet MUST be dropped and an alert
// //    MAY be logged.  This process is summarized in Figure 3.
// //             +----------------+
// //             |        [0..3] -+--> forward to STUN
// //             |                |
// //             |      [16..19] -+--> forward to ZRTP
// //             |                |
// // packet -->  |      [20..63] -+--> forward to DTLS
// //             |                |
// //             |      [64..79] -+--> forward to TURN Channel
// //             |                |
// //             |    [128..191] -+--> forward to RTP/RTCP
UDP_MSG_TYPE WebRtcServer::detectMsgType(uint8_t * data, size_t len)
{
    if (data[0] >= 0 && data[0] <= 3) {
        return UDP_MSG_STUN;
    } else if (data[0] >= 16 && data[0] <= 19) {
        return UDP_MSG_ZRTP;
    } else if (data[0] >= 20 && data[0] <= 63) {
        return UDP_MSG_DTLS;
    } else if (data[0] >= 64 && data[0] <= 79) {
        return UDP_MSG_TURN;
    } else if (data[0] >= 128 && data[0] <= 191) {
        return UDP_MSG_RTP;
    }
    return UDP_MSG_UNKNOWN;
}

void WebRtcServer::stop()
{
    WebsocketServer::stop();
}