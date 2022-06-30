#include <iostream>
#include "rtp_media_source.hpp"
using namespace mms;

bool RtpMediaSource::onAudioPacket(std::shared_ptr<RtpPacket> audio_pkt) 
{
    // std::cout << "on rtp audio pkt, mark:" << (uint32_t)audio_pkt->header_.marker << std::endl;
    return true;
}

bool RtpMediaSource::onVideoPacket(std::shared_ptr<RtpPacket> video_pkt) 
{
    std::cout << "on rtp video pkt, mark:" << (uint32_t)video_pkt->header_.marker << ", payload_len:" << video_pkt->payload_len_ << std::endl;
    return true;
}