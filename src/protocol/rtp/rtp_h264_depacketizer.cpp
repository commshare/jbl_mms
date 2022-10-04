#include "rtp_h264_depacketizer.h"
using namespace mms;

std::shared_ptr<RtpH264NALU> RtpH264Depacketizer::onPacket(std::shared_ptr<H264RtpPacket> pkt)
{
    if (pkt->isSingleNALU())
    {// todo extrace h264 single nalu
        std::map<uint16_t, std::shared_ptr<H264RtpPacket>> m;
        m.insert(std::pair(pkt->getSeqNum(), pkt));
        std::shared_ptr<RtpH264NALU> out_pkt = std::make_shared<RtpH264NALU>();
        out_pkt->setRtpPkts(m);
        return out_pkt;
    }

    auto & pkts_map = time_rtp_pkts_buf_[pkt->getTimestamp()];
    pkts_map.insert(std::pair(pkt->getSeqNum(), pkt));
    auto it = pkts_map.begin();
    if (!it->second->isStartFU())
    {
        return nullptr;
    }

    bool find_end_fu = false;
    uint16_t prev_seq = it->second->getSeqNum();
    it++;
    size_t total_nalu_size = 0;
    while (it != pkts_map.end())
    {
        if (it->second->getSeqNum() != prev_seq + 1)
        {
            return nullptr;
        }

        if (it->second->isEndFU())
        {
            find_end_fu = true;
        }

        total_nalu_size += it->second->payload_len_ - 2;
        it++;
    }

    if (!find_end_fu)
    {
        return nullptr;
    }
    // 条件都具备了，输出H264NALU
    std::shared_ptr<RtpH264NALU> out_pkt = std::make_shared<RtpH264NALU>();
    out_pkt->setRtpPkts(pkts_map);
    // out_pkt->rbsp.resize(total_nalu_size);
    // size_t pos = 0;
    // for (it = pkts_map.begin(); it != pkts_map.end(); it++)
    // {
    //     memcpy((char*)out_pkt->rbsp.data() + pos, it->second->payload_ + 2, it->second->payload_len_ - 2);
    //     pos += it->second->payload_len_ - 2;
    // }
    return out_pkt;
}