#include <boost/shared_ptr.hpp>

#include "rtmp_server.hpp"

namespace mms {

void RtmpServer::onConnOpen(std::unique_ptr<RtmpConn> conn) {
    conn->doService();
}

void RtmpServer::onConnClosed(std::unique_ptr<RtmpConn> sock) {

}

};