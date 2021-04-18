#include "tcp_socket.hpp"

namespace mms {

TcpSocket::TcpSocket(ThreadWorker *worker) : socket_(worker->getIOContext()) {

}

TcpSocket::~TcpSocket() {
    
}


};