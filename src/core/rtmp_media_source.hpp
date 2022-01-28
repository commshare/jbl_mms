#pragma once
#include <vector>

#include "media_source.hpp"
#include "server/rtmp/rtmp_protocol/rtmp_define.hpp"

#include "pipeline/handler.h"
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
        createPipeLine();
    }

    virtual ~RtmpMediaSource() {

    }

    bool processPkt(std::shared_ptr<RtmpMessage> pkt) {
        return pipeline_.processPkt(pkt);
        return true;
    }

    void createPipeLine() {
    }
private:
    std::vector<std::shared_ptr<RtmpMessage>> pkts_;
    Pipeline<
        RtmpBandwidthHandler,
        RtmpCodecParser,
        RtmpStatsHandler
    > pipeline_;
};
};