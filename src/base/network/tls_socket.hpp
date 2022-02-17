#pragma once
#include "tcp_socket.hpp"

#include <memory>
#include <unordered_map>
#include <boost/asio/ip/tcp.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>

#include "openssl/ssl.h"
#include "openssl/bio.h"
#include "base/thread/thread_worker.hpp"
#include "base/network/tcp_socket.hpp"

namespace mms {
class TlsSocket : public TcpSocket {
public:
    TlsSocket(TcpSocketHandler *handler, boost::asio::ip::tcp::socket *sock, ThreadWorker *worker):TcpSocket(handler, sock, worker) {
    }

    ~TlsSocket() {
        if (ssl_) {
            SSL_free(ssl_);
        }
    }

    bool init(SSL_CTX *ssl_ctx) {
        ssl_ = SSL_new(ssl_ctx);
        SSL_set_fd(ssl_, socket_->native_handle());
        if (SSL_accept(ssl_) <= 0) {
            return false;
        }
        return true;
    }
private:
    BIO *network_bio_;
    SSL *ssl_ = nullptr;
};
};