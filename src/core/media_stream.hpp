#pragma once
#include <memory>
#include <vector>

#include "server/rtmp/rtmp_conn/rtmp_conn.hpp"
#include "server/rtmp/rtmp_protocol/rtmp_handshake.hpp"
#include "server/rtmp/rtmp_protocol/rtmp_chunk_protocol.hpp"

#include "base/thread/thread_worker.hpp"
namespace mms {
class MediaStream {
public:
    MediaStream(ThreadWorker *worker) : worker_(worker) {
        
    }
 
    void init() {
    
    }
 
    virtual ~MediaStream() {

    }

    inline ThreadWorker* getWorker() {
        return worker_;
    }

private:
    ThreadWorker *worker_;
    bool ready_;
};

};