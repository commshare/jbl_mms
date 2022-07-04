#pragma once
#include <map>
#include <memory>
#include "protocol/rtp/rtp_h264_packet.h"
namespace mms {
class RtpH264NALU {

private:
    std::map<uint16_t, std::shared_ptr<H264RtpPacket>> rtp_pkts_;
};
};