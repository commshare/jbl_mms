
#include <sys/prctl.h>

#include <functional>
#include <thread>
#include <atomic>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <boost/asio.hpp>
namespace mms {
class ThreadWorker {
public:
    using Task = std::function<void()>;
    ThreadWorker() {
        running_ = false;
    }

    virtual ~ThreadWorker() {

    }
    // 设置在哪个cpu核心上运行
    void setCPUCore(int cpu_core) {
        cpu_core_ = cpu_core;
    }

    int getCPUCore() {
        return cpu_core_;
    }

    template<typename F, typename ...ARGS>
    void post(F &&f, ARGS &&...args) {
        io_service_.post(std::bind(f, std::forward<ARGS>(args)...));
    }

    template<typename F, typename ...ARGS>
    void dispatch(F &&f, ARGS &&...args) {
        io_service_.dispatch(std::bind(f, std::forward<ARGS>(args)...));
    }
    
    void start() {
        if (running_) {
            return;
        }
        running_ = true;
        thread_ = std::thread(std::bind(&ThreadWorker::work, this));
    }

    void work() {
        cpu_set_t mask;
        CPU_ZERO(&mask);
        CPU_SET(cpu_core_, &mask);
        if (pthread_setaffinity_np(pthread_self(), sizeof(mask), &mask) < 0) {
            // print error log
        } else {

        }
        work_ = std::make_shared<boost::asio::io_service::work>(io_service_);
        io_service_.run();
        std::cout << "stop worker:" << cpu_core_ << " ..." << std::endl;
    }

    void stop() {
        if (!running_) {
            return;
        }

        io_service_.stop();
        thread_.join();
    }

    boost::asio::io_service & getIOService() {
        return io_service_;
    }
private:
    int cpu_core_;
    boost::asio::io_service io_service_;
    std::shared_ptr<boost::asio::io_service::work> work_;
    std::thread thread_;
    std::atomic_bool running_;
};

};