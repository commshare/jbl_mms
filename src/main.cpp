#include <iostream>

#include <boost/asio.hpp>  
#include <boost/date_time/posix_time/posix_time.hpp>

#include "system/system.h"

int main(char argc, char *argv[]) {
    boost::asio::io_service io;  
    boost::asio::deadline_timer t(io, boost::posix_time::seconds(5));  
    t.wait();  
    std::cout << "cpu count:" << mms::getCPUCount() << std::endl;
    return 0;
}