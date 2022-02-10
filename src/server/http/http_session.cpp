#include <iostream>
#include "http_session.hpp"

namespace mms {
HttpSession::HttpSession(HttpConn *conn):conn_(conn), RtmpMediaSink(conn->getWorker()) {
    conn_->session_ = this;
}

HttpSession::~HttpSession() {
}

void HttpSession::service() {
    boost::asio::spawn(conn_->getWorker()->getIOContext(), [this](boost::asio::yield_context yield) {
        http_parser_.onHttpRequest([this](std::shared_ptr<HttpRequest> req) {
            // 得到session name，将session加到source里面，结束
        });

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
    });
}

bool HttpSession::sendRtmpMessage(std::shared_ptr<RtmpMessage> pkt) {

}

void HttpSession::close() {

}

};