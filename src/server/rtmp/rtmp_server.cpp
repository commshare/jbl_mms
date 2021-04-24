#include <boost/shared_ptr.hpp>

#include "rtmp_server.hpp"
#include "rtmp_session.hpp"
namespace mms {

void RtmpServer::onConnOpen(RtmpConn *conn) {
    RtmpSession *s = new RtmpSession(conn);
    s->service();
}

void RtmpServer::onConnClosed(RtmpConn *conn) {

}

};