#include <iostream>
#include "rtp_media_source.hpp"
using namespace mms;

bool RtpMediaSource::onAudioPacket(std::shared_ptr<RtpPacket> audio_pkt) 
{
    std::cout << "on rtp audio pkt" << std::endl;
    return true;
}

bool RtpMediaSource::onVideoPacket(std::shared_ptr<RtpPacket> video_pkt) 
{
    std::cout << "on rtp video pkt" << std::endl;
    return true;
}