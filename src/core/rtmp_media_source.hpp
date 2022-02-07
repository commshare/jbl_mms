#pragma once
#include <vector>
#include <set>

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
class RtmpMediaSource : public MediaSource {
public:
    RtmpMediaSource(ThreadWorker *worker) : MediaSource(worker), av_pkts_(2048) {

    }

    bool init() {
        return true;
    }

    virtual ~RtmpMediaSource() {

    }

    bool processPkt(std::shared_ptr<RtmpMessage> pkt) {
        return true;
    }

    bool onAudioPacket(std::shared_ptr<RtmpMessage> audio_pkt) {
        std::lock_guard<std::mutex> lck(sinks_mtx_);
        for (auto & sink : sinks_) {
            sink->getWorker()->post([]{

            });
            // sink->onAudioPacket(audio_pkt);
        }
        
        return true;
    }

    bool onVideoPacket(std::shared_ptr<RtmpMessage> video_pkt) {
        av_pkts_.addPkt(video_pkt);
        std::lock_guard<std::mutex> lck(sinks_mtx_);
        for (auto & sink : sinks_) {
            // sink->onVideoPacket(video_pkt);
        }
        return true;
    }

    bool onMetadata(std::shared_ptr<RtmpMetaDataMessage> metadata_pkt) {
        metadata_ = metadata_pkt;
        return true;
    }

    bool addMediaSink(std::shared_ptr<MediaSink> media_sink) {
        std::lock_guard<std::mutex> lck(sinks_mtx_);
        sinks_.insert(media_sink);
    }
private:
    std::mutex sinks_mtx_;
    std::set<std::shared_ptr<MediaSink>> sinks_;

    RtmpFlvDemuxer rtmp_flv_demuxer_;
    SequencePktBuf<RtmpMessage> av_pkts_;
    Pipeline<
        RtmpCodecParser,
        RtmpBandwidthHandler,
        RtmpStatsHandler
    > pipeline_;

    std::shared_ptr<RtmpMetaDataMessage> metadata_;
    std::shared_ptr<RtmpMessage> video_header_;
    std::shared_ptr<RtmpMessage> audio_header_;
private:
    bool has_video_; 
    bool has_audio_;
    bool stream_ready_;
};
};