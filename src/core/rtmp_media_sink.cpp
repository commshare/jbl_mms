#include "rtmp_media_sink.hpp"
#include "rtmp_media_source.hpp"

using namespace mms;

void RtmpMediaSink::active() {
    boost::asio::spawn(worker_->getIOContext(), [this](boost::asio::yield_context y) {
        if (!source_->isReady()) {
            return;
        }

        auto s = (RtmpMediaSource*)source_;
        auto v = s->getPkts(last_send_pkt_index_, 10);
        if (v.size() > 0) {
            last_send_pkt_index_++;
            for(auto p : v) {
                onRtmpPacket(p, y);
            }
            active();
        }
    });
}