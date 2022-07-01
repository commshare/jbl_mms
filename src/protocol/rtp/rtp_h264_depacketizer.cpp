#include "rtp_h264_depacketizer.h"
using namespace mms;

std::shared_ptr<H264NALU> RtpH264Depacketizer::onPacket(std::shared_ptr<H264RtpPacket> pkt)
{
    auto & pkts_map = time_rtp_pkts_buf_[pkt->getTimestamp()];
    pkts_map.insert(std::pair(pkt->getSeqNum(), pkt));

    for (auto it = pkts_map.begin(); it != pkts_map.end(); it++)
    {
        // if (it->second->)
    }
}