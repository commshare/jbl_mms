#include <boost/shared_ptr.hpp>
#include <memory>

#include "rtmp_server.hpp"
#include "rtmp_session.hpp"
namespace mms {
// conn属于server,session属于conn
void RtmpServer::onTcpSocketOpen(TcpSocket *conn, boost::asio::yield_context & yield) {
    RtmpConn *rtmp_conn = (RtmpConn*)conn;
    std::shared_ptr<RtmpSession> s = rtmp_conn->createSession();
    s->service(yield);
}

void RtmpServer::onTcpSocketClose(TcpSocket *conn, boost::asio::yield_context & yield) {
    RtmpConn *rtmp_conn = (RtmpConn*)conn;
    std::shared_ptr<RtmpSession> s = rtmp_conn->getSession();
    s->close();
}

};