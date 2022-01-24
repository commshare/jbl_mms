#include <boost/shared_ptr.hpp>
#include <memory>

#include "rtmp_server.hpp"
#include "rtmp_session.hpp"
namespace mms {

void RtmpServer::onConnOpen(RtmpConn *conn) {
    std::shared_ptr<RtmpSession> s = std::make_shared<RtmpSession>(conn);
    s->service();
}

void RtmpServer::onConnClosed(RtmpConn *conn) {

}

};