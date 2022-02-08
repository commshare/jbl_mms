#pragma once
#include <memory>

#include "base/thread/thread_worker.hpp"
namespace mms {
class MediaSource;
class MediaSink {
    friend class MediaSource;
public:
    MediaSink(ThreadWorker *worker) : worker_(worker) {

    }

    void setSource(MediaSource *source);

    virtual ~MediaSink() {

    }

    virtual void active() {

    }

    virtual void close() {

    }
protected:
    MediaSource *source_;
    ThreadWorker *worker_;
};

};