#include "rtmp_server.hpp"

namespace mms {

void RtmpServer::onTcpSocketOpen(boost::shared_ptr<TcpSocket> sock) {
    RtmpServerContext *ctx = new RtmpServerContext(sock);
    ctx->run();
}

void RtmpServer::onTcpSocketClosed(boost::shared_ptr<TcpSocket> sock) {

}

};