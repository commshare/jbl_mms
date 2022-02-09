#include <boost/shared_ptr.hpp>
#include <memory>

#include "http_server.hpp"
#include "http_session.hpp"
namespace mms {

void HttpServer::onTcpSocketOpen(TcpSocket *conn, boost::asio::yield_context & yield) {
    HttpConn * http_conn = (HttpConn*)conn;
    std::shared_ptr<HttpSession> s = std::make_shared<HttpSession>(http_conn);
    s->service(yield);
}

void HttpServer::onTcpSocketClose(TcpSocket *conn, boost::asio::yield_context & yield) {

}

};