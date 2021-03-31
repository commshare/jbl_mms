
#include <sys/prctl.h>

#include <functional>
#include <thread>
#include <atomic>
#include <mutex>
#include <queue>
#include <condition_variable>
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

    void addTask(const Task & t) {
        std::unique_lock<std::mutex> lock(tasks_mutex_);
        tasks_.push(t);
        tasks_cv_.notify_one();
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
        
        while(running_) {
            std::unique_lock<std::mutex> lock(tasks_mutex_);
            tasks_cv_.wait(lock, [this]() {return !running_ || !tasks_.empty();});
            if (!running_) {
                return;
            }

            if (tasks_.empty()) {
                continue;
            }
            Task t = tasks_.front();
            tasks_.pop();
            t();
        }
    }

    void stop() {
        if (!running_) {
            return;
        }

        running_ = false;
        tasks_cv_.notify_one();
        thread_.join();
    }
private:
    int cpu_core_;
    std::mutex tasks_mutex_;
    std::condition_variable tasks_cv_;
    std::queue<Task> tasks_;
    std::thread thread_;
    std::atomic_bool running_;
};

};