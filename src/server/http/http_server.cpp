#include <boost/shared_ptr.hpp>
#include <memory>

#include "http_server.hpp"
#include "http_session.hpp"
namespace mms {

void HttpServer::onTcpSocketOpen(TcpSocket *conn) {
    HttpConn * http_conn = (HttpConn*)conn;
    std::shared_ptr<HttpSession> s = http_conn->createSession();
    s->service();
}

void HttpServer::onTcpSocketClose(TcpSocket *conn) {
    HttpConn *http_conn = (HttpConn*)conn;
    std::shared_ptr<HttpSession> s = http_conn->getSession();
    http_conn->destroySession();
    if (s) {
        s->close();
    }
}

};