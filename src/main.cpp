#include <iostream>

#include <boost/asio.hpp>  
#include <boost/date_time/posix_time/posix_time.hpp>

#include "base/thread/thread_pool.hpp"
using namespace mms;
int main(int argc, char *argv[]) {
    ThreadPool p;
    p.start(std::thread::hardware_concurrency());
    auto w = p.getWorker(0);
    std::cout << "cpu count:" << std::thread::hardware_concurrency() << std::endl;
    w->post([]() {
        std::cout << "running post task" << std::endl;
    });

    w->dispatch([]() {
        std::cout << "running dispatch task" << std::endl;
    });
    boost::asio::io_service io;
    boost::asio::deadline_timer t(io, boost::posix_time::seconds(2));  
    t.wait();
    p.stop();
    return 0;
}