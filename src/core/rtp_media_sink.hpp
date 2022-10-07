#pragma once
#include "media_sink.hpp"
#include "protocol/rtp/rtp_packet.h"
namespace mms {
class RtpMediaSink : public MediaSink {
public:
    RtpMediaSink(ThreadWorker *worker);
    bool onAudioPacket(std::shared_ptr<RtpPacket> audio_pkt);
    bool onVideoPacket(std::shared_ptr<RtpPacket> video_pkt);
};

};