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

        return true;
    }

    void createPipeLine() {
        pipeline_codec_parser_.connectPipeLine(
            pipeline_bandwidth_handler_,
            pipeline_stats_handler_
        );
    }
private:
    std::vector<std::shared_ptr<RtmpMessage>> pkts_;
    Pipeline<RtmpCodecParser> pipeline_codec_parser_;
    Pipeline<RtmpBandwidthHandler> pipeline_bandwidth_handler_;
    Pipeline<RtmpStatsHandler> pipeline_stats_handler_;
};
};