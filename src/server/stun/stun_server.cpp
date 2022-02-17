#include "stun_server.hpp"
using namespace mms;

void StunServer::onTcpSocketOpen(TcpSocket *socket) {
    TlsSocket *tls_socket = (TlsSocket*)socket;
}

void StunServer::onTcpSocketClose(TcpSocket *socket) {

}