#pragma once
#include <vector>

#include "media_source.hpp"
#include "server/rtmp/rtmp_protocol/rtmp_define.hpp"

#include "pipeline/pipeline.h"
#include "server/rtmp/handlers/rtmp_codec_parser.hpp"
#include "server/rtmp/handlers/rtmp_bandwidth_handler.hpp"
#include "server/rtmp/handlers/rtmp_stats_handler.hpp"

namespace mms {
class RtmpMediaSource : public MediaSource<RtmpMessage> {
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

private:
    std::shared_ptr<RtmpMessage> metadata_;
    std::vector<std::shared_ptr<RtmpMessage>> pkts_;
    Pipeline<
        RtmpCodecParser,
        RtmpBandwidthHandler,
        RtmpStatsHandler
    > pipeline_;
};
};