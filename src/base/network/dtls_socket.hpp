#pragma once
#include "udp_socket.hpp"
namespace mms
{
    class DtlsSocket : public UdpSocket
    {
    public:
        DtlsSocket();
        virtual ~DtlsSocket();
    private:
        void onUdpSocketRecv(UdpSocket *sock, std::unique_ptr<uint8_t[]> data, size_t len, boost::asio::ip::udp::endpoint &remote_ep);
    };
};