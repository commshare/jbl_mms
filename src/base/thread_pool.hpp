#include <sys/prctl.h>

#include <functional>
#include <thread>
#include <memory>
#include <atomic>

#include "thread_worker.hpp"
namespace mms {
class ThreadPool {
public:
    ThreadPool() : running_(false) {

    }
    virtual ~ThreadPool() {

    }

    void start(int cpu_count) {
        if (running_) {
            return;
        }
        running_ = true;
        workers_.reserve(cpu_count);
        for (int i = 0; i < cpu_count; i++) {
            ThreadWorker *w = new ThreadWorker();
            w->setCPUCore(i);
            w->start();
            workers_.emplace_back(w);
        }
    }

    void stop() {
        if (!running_) {
            return;
        }
        running_ = false;
        for (auto & w : workers_) {
            w->stop();
            delete w;
        }
    }

    ThreadWorker* getWorker(int cpu_num) {
        return workers_[cpu_num];
    }
private:
    std::atomic_bool running_;
    std::vector<ThreadWorker*> workers_;
};
};