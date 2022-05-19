#include "webrtc_server.hpp"
#include "websocket_conn.hpp"
#include "webrtc_session.hpp"
#include "config/config.h"

#include "server/stun/protocol/stun_binding_response_msg.hpp"
#include "server/stun/protocol/stun_mapped_address_attr.h"
#include "udp_msg_demultiplex.hpp"

using namespace mms;

bool WebRtcServer::start()
{
    bool ret = WebsocketServer::start();
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

void WebRtcServer::onUdpSocketRecv(UdpSocket *sock, std::unique_ptr<uint8_t[]> data, size_t len, boost::asio::ip::udp::endpoint &remote_ep)
{
    auto worker = thread_pool_inst::get_mutable_instance().getWorker(-1);
    std::cout << "webrtc server recv len:" << len << std::endl;
    boost::asio::spawn(worker->getIOContext(), [this, sock, recv_data = std::move(data), len, remote_ep](boost::asio::yield_context yield) {
        uint8_t *data = recv_data.get();
        if (detectMsgType(data, len) == UDP_MSG_STUN) {
            StunMsg stun_msg;
            int32_t ret = stun_msg.decode(data, len);
            if (0 == ret) 
            {
                if (processStunPacket(stun_msg, data, len, sock, remote_ep, yield)) 
                {
                    return;
                }
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
        std::lock_guard<std::mutex> lck(ufrag_session_map_mtx_);
        auto it_session = ufrag_session_map_.find(local_user_name);
        if (it_session == ufrag_session_map_.end())
        {
            return false;
        }
        session = it_session->second;
    }

    if (!session) //todo add log
    {
        return false;
    }
    return session->processStunPacket(stun_msg, data, len, sock, remote_ep, yield);
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
            std::lock_guard<std::mutex> lck(ufrag_session_map_mtx_);
            ufrag_session_map_.insert(std::pair(webrtc_session->getLocalICEUfrag(), webrtc_session));
        }

        conn->set_message_handler(std::bind(&WebRtcSession::onMessage, webrtc_session.get(), this, websocketpp::lib::placeholders::_1, websocketpp::lib::placeholders::_2));

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