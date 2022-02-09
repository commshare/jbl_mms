#include "rtmp_media_sink.hpp"
#include "rtmp_media_source.hpp"

using namespace mms;

void RtmpMediaSink::active() {
    boost::asio::spawn(worker_->getIOContext(), [this](boost::asio::yield_context yield) {
        if (!source_->isReady()) {
            return;
        }
        
        if (sending_) {
            return;
        }
        sending_ = true;
        auto s = (RtmpMediaSource*)source_;
        auto v = s->getPkts(last_send_pkt_index_, 10);
        // std::cout << "***************** get pkts size:" << v.size() << " *******************" << std::endl;
        if (v.size() > 0) {
            for(auto p : v) {
                if (!onRtmpPacket(p, yield)) {
                    break;
                }
            }
            // active();
        }
        sending_ = false;
    });
}