#pragma once
#include "protocol/flv/flv_define.hpp"
#include "server/rtmp/rtmp_protocol/rtmp_define.hpp"
namespace mms {
// rtmp数据只包含tagdata，严格来说不是flv，所以称rtmpflv
class RtmpFlvDemuxer {
public:
    bool isKeyFrame(RtmpMessage *rtmp_msg) {
        // 只需要解析头部就可以了
        VideoTagHeader video_tag_header;
        int32_t consumed = video_tag_header.decode(rtmp_msg->payload_, rtmp_msg->payload_size_);
        if (consumed < 0) {
            return false;
        }

        if (video_tag_header.avc_packet_type == VideoTagHeader::AVCNALU) {
            if (video_tag_header.frame_type == VideoTagHeader::KeyFrame) {
                return true;
            }
        }
        return false;
    }
};
};