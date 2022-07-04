#pragma once
#include <memory>
#include <unordered_map>
#include <map>

#include "rtp_h264_packet.h"
#include "codec/rtp_h264_nalu.h"
// 参考算法：https://blog.csdn.net/u010178611/article/details/82625891
namespace mms {
class RtpH264Depacketizer {
public:
    RtpH264Depacketizer(){};
    virtual ~RtpH264Depacketizer() {};
    std::shared_ptr<RtpH264NALU> onPacket(std::shared_ptr<H264RtpPacket> pkt);
private:
    std::unordered_map<uint32_t, std::map<uint16_t, std::shared_ptr<H264RtpPacket>>> time_rtp_pkts_buf_;
};
};