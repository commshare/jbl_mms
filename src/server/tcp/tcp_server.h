#include <stdint.h>
#include <string>

namespace mms {
class TcpServer {
public:
    TcpServer()
    {

    }
    virtual ~TcpServer()
    {

    }
public:
    void listen(uint16_t port, const std::string & addr = "");
    
};
};