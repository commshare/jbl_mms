#include <boost/shared_ptr.hpp>
#include <memory>

#include "http_server.hpp"
#include "http_session.hpp"
namespace mms {

void HttpServer::onConnOpen(HttpConn *conn) {
    std::shared_ptr<HttpSession> s = std::make_shared<HttpSession>(conn);
    s->service();
}

void HttpServer::onConnClosed(HttpConn *conn) {

}

};