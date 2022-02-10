#include <boost/shared_ptr.hpp>
#include <memory>

#include "rtmp_server.hpp"
#include "rtmp_session.hpp"
namespace mms {
// conn属于server,session属于conn
void RtmpServer::onTcpSocketOpen(TcpSocket *conn) {
    RtmpConn *rtmp_conn = (RtmpConn*)conn;
    std::shared_ptr<RtmpSession> s = rtmp_conn->createSession();
    s->service();
}

void RtmpServer::onTcpSocketClose(TcpSocket *conn) {
    RtmpConn *rtmp_conn = (RtmpConn*)conn;
    std::shared_ptr<RtmpSession> s = rtmp_conn->getSession();
    rtmp_conn->destroySession();
    if (s) {
        s->close();
    }
}

};