#pragma once
namespace mms {
class UdpSocket;
class UdpSocketHandler {
public:
    virtual void onUdpSocketRecv(UdpSocket *sock) = 0;
};

class UdpSocket {

};
};