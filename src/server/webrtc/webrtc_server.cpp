#include "webrtc_server.hpp"
#include "websocket_conn.hpp"
#include "webrtc_session.hpp"

using namespace mms;

bool WebRtcServer::start() {
    bool ret = WebsocketServer::start();
    if (!ret) {
        return false;
    }

    ret = UdpServer::startListen(2022);
    if (!ret) {
        return false;
    }   
    return true;
}

void WebRtcServer::onUdpSocketRecv(UdpSocket *sock, std::unique_ptr<uint8_t[]> data, size_t len, boost::asio::ip::udp::endpoint &remote_ep) {
    auto worker = thread_pool_inst::get_mutable_instance().getWorker(-1);
    std::cout << "recv len:" << len << std::endl;
    boost::asio::spawn(worker->getIOContext(), [this, sock, recv_data = std::move(data), len, remote_ep](boost::asio::yield_context yield) {
    });
}

void WebRtcServer::onWebsocketOpen(websocketpp::connection_hdl hdl) {
    try {
        websocketpp::server<websocketpp::config::asio>::connection_ptr conn = get_con_from_hdl(hdl);
        std::shared_ptr<WebSocketConn> ws_conn = std::make_shared<WebSocketConn>(worker_, conn); 
        auto webrtc_session = ws_conn->createSession();
        {
            std::lock_guard<std::mutex> lck(mtx_);
            conn_map_.insert(std::pair(conn, ws_conn));
        }
        
        conn->set_message_handler(std::bind(&WebRtcSession::onMessage, webrtc_session.get(), this, websocketpp::lib::placeholders::_1, websocketpp::lib::placeholders::_2));

        webrtc_session->service();
    } catch (std::exception & e) {
        std::cout << "open websocket failed." << std::endl;
    }
}

void WebRtcServer::onWebsocketClose(websocketpp::connection_hdl hdl) {
    websocketpp::server<websocketpp::config::asio>::connection_ptr conn = get_con_from_hdl(hdl);
    std::shared_ptr<WebSocketConn> ws_conn;
    {
        std::lock_guard<std::mutex> lck(mtx_);
        auto it = conn_map_.find(conn);
        if (it == conn_map_.end()) {
            return;
        }
        ws_conn = it->second;
    }
    
    conn_map_.erase(conn);
}