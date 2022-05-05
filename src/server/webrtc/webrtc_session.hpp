#pragma once
#include <memory>

#include "base/thread/thread_worker.hpp"
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include "core/session.hpp"
#include "protocol/sdp/sdp.hpp"

namespace mms {
class WebsocketServer;
class WebSocketConn;
class WebRtcSession  : public Session, public std::enable_shared_from_this<WebRtcSession>{
public:
    WebRtcSession(ThreadWorker *worker, WebSocketConn *conn);
    virtual ~WebRtcSession();

    void service();
    void close() override;
    ThreadWorker *getWorker() {
        return worker_;
    }
    void onMessage(websocketpp::server<websocketpp::config::asio>* server, websocketpp::connection_hdl hdl, websocketpp::server<websocketpp::config::asio>::message_ptr msg);
private:
    bool processOfferMsg(const std::string & sdp);
    int32_t createLocalSdp();
private:
    ThreadWorker *worker_;
    WebSocketConn *ws_conn_;
    Sdp remote_sdp_;
    Sdp local_sdp_;
    uint64_t session_id_ = 0;
    std::string app_;
    std::string stream_;
    std::string ice_ufrag_;
    std::string ice_pwd_;
};

};