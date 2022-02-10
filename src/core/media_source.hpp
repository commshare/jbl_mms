#pragma once
#include <memory>
#include <vector>
#include <mutex>
#include <set>

#include "media_source.hpp"
#include "media_sink.hpp"

#include "server/rtmp/rtmp_conn/rtmp_conn.hpp"
#include "server/rtmp/rtmp_protocol/rtmp_handshake.hpp"
#include "server/rtmp/rtmp_protocol/rtmp_chunk_protocol.hpp"

namespace mms {
class MediaSink;
class MediaSource {
    friend class MediaSink;
public:
    MediaSource(ThreadWorker *worker) : worker_(worker){
        
    }

    virtual ~MediaSource() {

    }

    virtual bool addMediaSink(std::shared_ptr<MediaSink> media_sink) = 0;
    virtual bool removeMediaSink(std::shared_ptr<MediaSink> media_sink) = 0;
    bool isReady() {
        return stream_ready_;
    }
protected:
    bool stream_ready_ = false;
    ThreadWorker *worker_;
    std::mutex sinks_mtx_;
    std::set<std::shared_ptr<MediaSink>> sinks_;
};

};