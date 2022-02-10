#pragma once
#include <memory>
#include <boost/array.hpp>

#include "base/shared_ptr.hpp"
#include "server/http/http_conn.hpp"
#include "server/http/http_protocol/http_parser.hpp"
#include "server/http/http_protocol/http_define.hpp"
#include "core/session.hpp"
#include "core/rtmp_media_sink.hpp"

namespace mms {
class HttpSession : public Session, public RtmpMediaSink, public std::enable_shared_from_this<HttpSession> {
public:
    HttpSession(HttpConn *conn);
    virtual ~HttpSession();
    void service();
    void close();
private:
    bool sendRtmpMessage(std::shared_ptr<RtmpMessage> pkt) override;
    HttpConn *conn_;
    HttpParser http_parser_;
};

};