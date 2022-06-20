#pragma once
#include <vector>
#include <set>
#include <atomic>

#include "media_source.hpp"
#include "media_sink.hpp"

#include "server/rtmp/rtmp_protocol/rtmp_define.hpp"

#include "pipeline/pipeline.h"
#include "server/rtmp/handlers/rtmp_codec_parser.hpp"
#include "server/rtmp/handlers/rtmp_bandwidth_handler.hpp"
#include "server/rtmp/handlers/rtmp_stats_handler.hpp"
#include "server/rtmp/rtmp_message/data_message/rtmp_metadata_message.hpp"

#include "demuxer/rtmp_flv_demuxer.hpp"
#include "base/thread/thread_worker.hpp"
#include "base/sequence_pkt_buf.hpp"


namespace mms {
class RtmpMediaSink : public MediaSink {
public:
    RtmpMediaSink(ThreadWorker *worker) : MediaSink(worker) {
    }

    virtual bool init() {
        return true;
    }

    virtual ~RtmpMediaSink() {

    }

    virtual bool onAudioPacket(std::shared_ptr<RtmpMessage> audio_pkt) {
        return true;
    }

    virtual bool onVideoPacket(std::shared_ptr<RtmpMessage> video_pkt) {
        return true;
    }

    virtual bool sendRtmpMessage(std::shared_ptr<RtmpMessage> pkt) {
        return true;
    }

    virtual bool startSendRtmpMessage() {
        return true;
    }

    bool onMetadata(std::shared_ptr<RtmpMetaDataMessage> metadata_pkt) {
        return true;
    } 
    
    void active() final;

    void close() {
    }
protected:
    int64_t last_send_pkt_index_ = -1;
    bool has_video_; 
    bool has_audio_;
    bool stream_ready_;
};
};