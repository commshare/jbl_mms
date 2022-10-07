#pragma once
#include <map>
#include <memory>
#include "protocol/rtp/rtp_h264_packet.h"
#include "h264_nalu.h"
namespace mms {
class RtpH264NALU : public H264NALU {
public:
    void setRtpPkts(const std::map<uint16_t, std::shared_ptr<H264RtpPacket>> & pkts) 
    {
        rtp_pkts_ = pkts;
    }

    size_t getPktsCount() {
        return rtp_pkts_.size();
    }

    std::map<uint16_t, std::shared_ptr<H264RtpPacket>> & getRtpPkts() {
        return rtp_pkts_;
    }
private:
    std::map<uint16_t, std::shared_ptr<H264RtpPacket>> rtp_pkts_;
};
};