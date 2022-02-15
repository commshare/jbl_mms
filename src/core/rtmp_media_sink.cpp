#include "rtmp_media_sink.hpp"
#include "rtmp_media_source.hpp"

using namespace mms;

void RtmpMediaSink::active() {
    worker_->post([this]()->boost::asio::awaitable<void> {
        if (!source_->isReady()) {
            co_return;
        }

        auto rtmp_source = (RtmpMediaSource*)source_;
        auto pkts = rtmp_source->getPkts(last_send_pkt_index_, 10);
        if (pkts.size() > 0) {
            for(auto pkt : pkts) {
                if (!co_await sendRtmpMessage(pkt)) {
                    break;
                }
            }
            startSendRtmpMessage();
        }
    });
}