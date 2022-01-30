#pragma once
#include <memory>
#include "base/shared_ptr.hpp"
#include "server/http/http_conn.hpp"
#include "core/session.hpp"

namespace mms {
class HttpSession : public Session {
public:
    HttpSession(HttpConn *conn);

    virtual ~HttpSession() {

    }

    void service();
    void close();
    HttpConn *conn_;
};

};