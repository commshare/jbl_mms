#include <atomic>
#include <boost/asio.hpp>

#include "../thread/thread_pool.hpp"

namespace mms {
class TcpSocket {
public:
    TcpSocket(ThreadWorker *worker_) {

    }

    virtual ~TcpSocket() {

    }


private:
    std::shared_ptr<boost::asio::ip::tcp::socket> socket_;
    std::atomic_uint64_t in_bytes_;
    std::atomic_uint64_t out_bytes_;
    ThreadWorker *worker_;
};
};