#include <iostream>

#include <boost/asio.hpp>  
#include <boost/date_time/posix_time/posix_time.hpp>

#include "system/system.h"
#include "base/thread_pool.hpp"
using namespace mms;
int main(char argc, char *argv[]) {
    ThreadPool p;
    p.start(mms::getCPUCount());
    auto w = p.getWorker(0);
    w->addTask([]() {
        std::cout << "running task" << std::endl;
    });
    boost::asio::io_service io;
    boost::asio::deadline_timer t(io, boost::posix_time::seconds(2));  
    t.wait();
    return 0;
}