#include "rtp_media_sink.hpp"
using namespace mms;

RtpMediaSink::RtpMediaSink(ThreadWorker *worker) : MediaSink(worker)
{

}

bool RtpMediaSink::onAudioPacket(std::shared_ptr<RtpPacket> audio_pkt)
{

}

bool RtpMediaSink::onVideoPacket(std::shared_ptr<RtpPacket> video_pkt)
{

}