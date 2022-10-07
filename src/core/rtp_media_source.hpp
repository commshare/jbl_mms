#pragma once
#include <vector>
#include <set>
#include <map>
#include <boost/circular_buffer.hpp>

#include "media_source.hpp"
#include "media_sink.hpp"

#include "pipeline/pipeline.h"

#include "base/thread/thread_worker.hpp"
#include "base/sequence_pkt_buf.hpp"

#include "protocol/rtp/rtp_packet.h"
#include "protocol/rtp/rtp_h264_depacketizer.h"


namespace mms {
class RtpMediaSource : public MediaSource {
public:
    RtpMediaSource(ThreadWorker *worker) : MediaSource(worker) {

    }

    bool init() {
        return true;
    }

    virtual ~RtpMediaSource() {

    }

    bool onAudioPacket(std::shared_ptr<RtpPacket> audio_pkt);
    bool onVideoPacket(std::shared_ptr<RtpPacket> video_pkt);

    bool addMediaSink(std::shared_ptr<MediaSink> media_sink) final {
        std::lock_guard<std::recursive_mutex> lck(sinks_mtx_);
        sinks_.insert(media_sink);
        media_sink->setSource(this);
        return true;
    }

    bool removeMediaSink(std::shared_ptr<MediaSink> media_sink) final {
        std::lock_guard<std::recursive_mutex> lck(sinks_mtx_);
        for (auto it = sinks_.begin(); it != sinks_.end(); it++) {
            if (*it == media_sink) {
                sinks_.erase(it);
                break;
            }
        }
        return true;
    }

    void close() {
        std::lock_guard<std::recursive_mutex> lck(sinks_mtx_);
        std::set<std::shared_ptr<MediaSink>> tmp_sinks_;
        tmp_sinks_.swap(sinks_);
        for (auto sink : tmp_sinks_) {
            sink->getWorker()->dispatch([sink]() {
                sink->close();
            });
        }
        sinks_.clear();
    }
protected:
    boost::circular_buffer<std::shared_ptr<RtpPacket>> av_pkts_;
    std::map<uint16_t, std::shared_ptr<RtpPacket>> video_pkts_;
    std::map<uint16_t, std::shared_ptr<RtpPacket>> audio_pkts_;
protected:
    bool has_video_; 
    bool video_ready_ = false;
    bool has_audio_;
    bool audio_ready_ = false;
    int32_t latest_video_timestamp_ = 0;
    int32_t latest_audio_timestamp_ = 0;

    RtpH264Depacketizer h264_depacketizer_;
};
};