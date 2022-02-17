#pragma once
#include "openssl/ssl.h"
#include "openssl/err.h"

#include "base/network/tls_socket.hpp"
#include "server/tcp/tcp_server.hpp"

namespace mms {
class TlsServer : public TcpServer<TlsSocket> {
public:
    TlsServer(ThreadWorker *worker);
    virtual ~TlsServer();
public:
    int32_t startListen(uint16_t port);
private:
    void onTcpSocketOpen(TcpSocket *socket) override;
    void onTcpSocketClose(TcpSocket *socket) override;
private:
    bool initSSL();
    bool createSSLContext();
    bool loadCertAndKey(const std::string & cert_file, const std::string & key_file);
    SSL_CTX *ssl_ctx_ = nullptr;
};
};