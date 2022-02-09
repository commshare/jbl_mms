#pragma once
#include <memory>
#include <boost/array.hpp>

#include "base/shared_ptr.hpp"
#include "server/http/http_conn.hpp"
#include "server/http/http_protocol/http_parser.hpp"
#include "server/http/http_protocol/http_define.hpp"
#include "core/session.hpp"

namespace mms {
class HttpSession : public Session {
public:
    HttpSession(HttpConn *conn);
    virtual ~HttpSession();
    void service(boost::asio::yield_context & yield);
    void close();
private:
    HttpConn *conn_;
    HttpParser http_parser_;
};

};