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


namespace mms {
class RtmpMediaSource : public MediaSource {
public:
    RtmpMediaSource() {

    }

    bool init() {
        return true;
    }

    virtual ~RtmpMediaSource() {

    }

    bool processPkt(std::shared_ptr<RtmpMessage> pkt) {
        pkts_.emplace_back(pkt);
        pipeline_.processPkt(pkt);
        return true;
    }

    bool onAudioPacket(std::shared_ptr<RtmpMessage> audio_pkt) {
        return true;
    }

    bool onVideoPacket(std::shared_ptr<RtmpMessage> video_pkt) {
        return true;
    }

    bool onMetadata(std::shared_ptr<RtmpMetaDataMessage> metadata_pkt) {
        metadata_ = metadata_pkt;
        return true;
    }
private:
    RtmpFlvDemuxer rtmp_flv_demuxer_;
    std::vector<std::shared_ptr<RtmpMessage>> pkts_;
    std::set<MediaSink*> sinks_;
    Pipeline<
        RtmpCodecParser,
        RtmpBandwidthHandler,
        RtmpStatsHandler
    > pipeline_;

    std::shared_ptr<RtmpMetaDataMessage> metadata_;
private:
    bool has_video_;
    bool has_audio_;
    bool stream_ready_;
};
};