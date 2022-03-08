#include <iostream>

#include "webrtc_session.hpp"
#include "websocket_server.hpp"
#include "json/json.h"

using namespace mms;
WebRtcSession::WebRtcSession(ThreadWorker *worker, WebSocketConn *conn) : worker_(worker), ws_conn_(conn) {
    std::cout << "create webrtcsession" << std::endl;
}

WebRtcSession::~WebRtcSession() {
    std::cout << "destroy webrtcsession" << std::endl;
}

void WebRtcSession::onMessage(websocketpp::server<websocketpp::config::asio>* server, websocketpp::connection_hdl hdl, websocketpp::server<websocketpp::config::asio>::message_ptr msg) {
    if (msg->get_opcode() == websocketpp::frame::opcode::text) {
        std::cout << "get text msg:" << msg->get_payload() << std::endl;
        Json::Value root;
        Json::Reader reader;
        if (!reader.parse(msg->get_payload(), root, false)) {
            std::cout << "not json msg" << std::endl;
            return;
        }

        if (!root.isMember("message") || !root["message"].isObject()) {
            std::cout << "msg is not object" << std::endl;
            return;
        }

        Json::Value & msg = root["message"];
        if (msg.isMember("type") || !msg["type"].isString()) {
            std::cout << "msg type is not string" << std::endl;
            return;
        }

        const std::string & type = msg["type"].asString();
        if ("offer" == type) {
            if (!msg.isMember("sdp") || !msg["sdp"].isString()) {
                std::cout << "no sdp info" << std::endl;
                return;
            }
            processOfferMsg(msg["sdp"].asString());
        }
    }
}

void WebRtcSession::processOfferMsg(const std::string & sdp) {
    
}

void WebRtcSession::service() {

}

void WebRtcSession::close() {

}

