#pragma once
#include <atomic>
#include <memory>

namespace mms {
template<typename T>
class SharedPtr {
private:
    SharedPtr() {
    }
public:
    virtual ~SharedPtr() {
    }

    uint32_t refCount() {
        return used_count_;
    }

    void reference() {
        used_count_++;
    }

    void dereference() {
        if (used_count_.fetch_sub(1) == 1) {
            delete this->ptr_;
            delete this;
        }
    }

    inline T * ptr() {
        return ptr_;
    }
    
    template<typename... ARGS>
    static SharedPtr<T> *makeShared(ARGS&&... args) {
        size_t total_size = sizeof(SharedPtr<T>) + sizeof(T);
        uint8_t *m = new uint8_t[total_size];
        SharedPtr<T> *s = new(m)SharedPtr<T>;
        s->ptr_ = (T*)(m + sizeof(SharedPtr<T>));
        s->used_count_ = 1;
        new(s->ptr_)T(std::forward<ARGS>(args)...);
        return s;
    }

public:
    std::atomic<uint32_t> used_count_;
    T *ptr_;
};
};