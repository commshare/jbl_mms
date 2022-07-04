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
    // std::cout << "on rtp video pkt, mark:" << (uint32_t)video_pkt->header_.marker << ", payload_len:" << video_pkt->payload_len_ << std::endl;
    auto h264_nalu = h264_depacketizer_.onPacket(std::static_pointer_cast<H264RtpPacket>(video_pkt));
    if (h264_nalu) {
        std::cout << "******************* get h264 nalu, rtp count:" << h264_nalu->getPktsCount() << " ********************" << std::endl;
    }
    return true;
}