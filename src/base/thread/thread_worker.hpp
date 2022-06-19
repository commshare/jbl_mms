
#pragma once

#include <sys/prctl.h>

#include <functional>
#include <thread>
#include <atomic>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace mms {
class ThreadWorker;
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
        io_context_.post(std::bind(f, std::forward<ARGS>(args)...));
    }

    template<typename F, typename ...ARGS>
    void dispatch(F &&f, ARGS &&...args) {
        io_context_.dispatch(std::bind(f, std::forward<ARGS>(args)...));
    }

    class Event {
    public:
        Event(ThreadWorker *worker, const std::function<void(Event *ev)> &f) : worker_(worker), f_(f), timer_(worker->getIOContext())
        {
        }

        void invokeAfter(uint32_t ms) {
            timer_.expires_from_now(boost::posix_time::milliseconds(ms));
            timer_.async_wait([this](const boost::system::error_code & ec) {
                f_(this);
            });
        }

    private:
        ThreadWorker *worker_;
        std::function<void(Event *ev)> f_;
        boost::asio::deadline_timer timer_;
    };

    Event* createEvent(const std::function<void(Event* ev)> &f)
    {
        Event* ev = new Event(this, f);
        return ev;
    }


    void removeEvent(Event *ev) 
    {
        delete ev;
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
        work_ = std::make_shared<boost::asio::io_context::work>(io_context_);
        io_context_.run();
    }

    void stop() {
        if (!running_) {
            return;
        }

        io_context_.stop();
        thread_.join();
    }

    boost::asio::io_context & getIOContext() {
        return io_context_;
    }
private:
    int cpu_core_;
    boost::asio::io_context io_context_;
    std::shared_ptr<boost::asio::io_service::work> work_;
    std::thread thread_;
    std::atomic_bool running_;
};

};