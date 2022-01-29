#pragma once
#include <atomic>
namespace mms {
template<typename T>
class RefObj {
public:
    RefObj():used_count_(1) {

    }

    void addRef() {
        used_count_++;
    }

    void subRef() {
        if (used_count_.fetch_sub(1) == 1) {
            delete this;
        }   
    }
protected:
    std::atomic<uint32_t> used_count_;
};

};