#include <iostream>
#include "http_session.hpp"
#include "http_flv.hpp"
#include "core/media_manager.hpp"
#include "server/http/http_protocol/http_response.hpp"

namespace mms {
HttpSession::HttpSession(HttpConn *conn):conn_(conn) {
}

HttpSession::~HttpSession() {
    std::cout << "******************* destroy HttpSession ******************" << std::endl;
}

void HttpSession::service() {
    auto self(shared_from_this());
    // todo:consider to wrap the conn as a bufio, and move parser to HttpRequest class.
    boost::asio::spawn(conn_->getWorker()->getIOContext(), [this, self](boost::asio::yield_context yield) {
        http_parser_.onHttpRequest(std::bind(&HttpSession::onHttpRequest, this, std::placeholders::_1));

        conn_->cycleRecv([this](const char *buf, size_t len, boost::asio::yield_context & yield)->int32_t {
            int32_t total_consumed = 0;
            int32_t consumed = 0;
            do {
                consumed = http_parser_.read(std::string(buf + total_consumed, len));
                if (consumed < 0) {
                    return -1;
                }
                total_consumed += consumed;
                len -= consumed;
            } while(consumed > 0 && len > 0);

            return total_consumed;
        }, yield);
        conn_->close();
        std::cout << "****************** end cycleRecv ******************" << std::endl;
    });
}

void HttpSession::onHttpRequest(std::shared_ptr<HttpRequest> req) {
    size_t pos;
    if ((pos = req->getPath().rfind(".flv")) != std::string::npos) {
        std::shared_ptr<HttpResponse> resp = std::make_shared<HttpResponse>(conn_);
        rtmp_media_sink_ = std::make_shared<HttpFlv>(this, req, resp);
        session_name_ = req->getHeader("Host") + req->getPath().substr(0, pos);
        std::cout << "session_name:" << session_name_ << std::endl;

        auto source = MediaManager::get_mutable_instance().getSource(session_name_);
        if (!source) {//todo : reply 404
            return;
        }
        std::cout << "****************** start init ********************" << std::endl;
        rtmp_media_sink_->init();
        source->addMediaSink(std::dynamic_pointer_cast<MediaSink>(rtmp_media_sink_));
    }
}

void HttpSession::close() {
    if (rtmp_media_sink_) {
        rtmp_media_sink_->close();
    }
}

};