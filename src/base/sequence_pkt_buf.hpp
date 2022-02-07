#pragma once
#include <atomic>
#include "boost/thread/mutex.hpp"
#include "boost/thread/thread.hpp"

namespace mms {
template<typename T>
class SequencePktBuf {
public:
    SequencePktBuf(size_t max_size) : indexes_(0) {
        pkt_buf_.resize(max_size);
        max_size_ = max_size;
    }

    uint64_t addPkt(std::shared_ptr<T> pkt) {
        boost::unique_lock<boost::shared_mutex> wlock(rw_mutex_);
        indexes_++;
        pkt_buf_[indexes_%max_size_] = std::make_pair(indexes_, pkt);
        return indexes_;
    }

    std::shared_ptr<T> getPkt(uint64_t index) {
        boost::shared_lock<boost::shared_mutex> rlock(rw_mutex_);
        return pkt_buf_[index%max_size_].second;
    }
private:
    size_t max_size_;
    boost::shared_mutex rw_mutex_;
    std::vector<std::pair<uint64_t, std::shared_ptr<T>>> pkt_buf_;
    uint64_t indexes_;
};

};