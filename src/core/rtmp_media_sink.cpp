#include "rtmp_media_sink.hpp"
#include "rtmp_media_source.hpp"

using namespace mms;

void RtmpMediaSink::active() {
    worker_->post([this]() {
        if (!source_->isReady()) {
            return;
        }

        auto rtmp_source = (RtmpMediaSource*)source_;
        auto pkts = rtmp_source->getPkts(last_send_pkt_index_, 10);
        if (pkts.size() > 0) {
            for(auto pkt : pkts) {
                if (!sendRtmpMessage(pkt)) {
                    break;
                }
            }
            startSendRtmpMessage();
        }
    });
}