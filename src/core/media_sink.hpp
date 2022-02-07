#pragma once
#include <memory>
#include "media_stream.hpp"
namespace mms {
class MediaSource;
class MediaSink : public MediaStream {
public:
    MediaSink(ThreadWorker *worker) : MediaStream(worker) {

    }

    void setSource(MediaSource *source);

    virtual ~MediaSink() {

    }

    virtual void active() {

    }
private:
    MediaSource *source_;
};

};