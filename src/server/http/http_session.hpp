#pragma once
#include <memory>
#include <boost/array.hpp>

#include "base/shared_ptr.hpp"
#include "server/http/http_conn.hpp"
#include "server/http/http_protocol/http_parser.hpp"
#include "server/http/http_protocol/http_define.hpp"
#include "server/http/http_protocol/http_request.hpp"
#include "core/session.hpp"
#include "core/rtmp_media_sink.hpp"

namespace mms {
class HttpSession : public Session, public std::enable_shared_from_this<HttpSession> {
public:
    HttpSession(HttpConn *conn);
    virtual ~HttpSession();
    void service();
    void close();
    boost::asio::awaitable<void> onHttpRequest(std::shared_ptr<HttpRequest> req);
    ThreadWorker *getWorker() {
        return conn_->getWorker();
    }
public:
    HttpConn *conn_;
    HttpParser http_parser_;
    std::shared_ptr<RtmpMediaSink> rtmp_media_sink_;
};

};