#include <iostream>
#include "http_session.hpp"

namespace mms {
HttpSession::HttpSession(HttpConn *conn):conn_(conn) {
    conn_->session_ = this;
}

HttpSession::~HttpSession() {
}

void HttpSession::service() {
    http_parser_.onHttpRequest([this](std::shared_ptr<HttpRequest> req) {
        std::cout << "get http req" << std::endl;
        std::cout << "path:" << req->path_ << std::endl;
        for (auto & p : req->params_) {
            std::cout << p.first << ":" << p.second << std::endl;
        }
    });

    conn_->cycleRecv([this](const char *buf, size_t len)->int32_t {
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
    });
}

void HttpSession::close() {

}

};