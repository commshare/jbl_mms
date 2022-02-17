#include "tls_server.hpp"

using namespace mms;

TlsServer::TlsServer(ThreadWorker *worker) : TcpServer(worker) {

}

TlsServer::~TlsServer() {
    if (ssl_ctx_) {
        SSL_CTX_free(ssl_ctx_);
    }
}

bool TlsServer::initSSL() {
    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();
    return true;
}

bool TlsServer::createSSLContext() {
    ssl_ctx_ = SSL_CTX_new(SSLv23_server_method());
    if (!ssl_ctx_) {
        return false;
    }

    return true;
}

bool TlsServer::loadCertAndKey(const std::string & cert_file, const std::string & key_file) {
    SSL_CTX_set_ecdh_auto(ssl_ctx_, 1);
    if (SSL_CTX_use_certificate_file(ssl_ctx_, cert_file.c_str(), SSL_FILETYPE_PEM) <= 0) {
        return false;
    }

    if (SSL_CTX_use_PrivateKey_file(ssl_ctx_, key_file.c_str(), SSL_FILETYPE_PEM) <= 0 ) {
        return false;
    }
    return true;
}

int32_t TlsServer::startListen(uint16_t port) {
    if (!initSSL()) {
        return -1;
    }

    if (!createSSLContext()) {
        return -2;
    }

    if (!loadCertAndKey("./certs/server_cert.pem", "./certs/server_key.pem")) {
        return -3;
    }

    return TcpServer::startListen(port);
}

void TlsServer::onTcpSocketOpen(TcpSocket *socket) {
    TlsSocket *tls_socket = (TlsSocket*)socket;
    if (!tls_socket->init(ssl_ctx_)) {

    }
}

void TlsServer::onTcpSocketClose(TcpSocket *socket) {

}
