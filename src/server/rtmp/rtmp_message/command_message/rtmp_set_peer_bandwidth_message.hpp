#pragma once
#include <stdint.h>
#include "server/rtmp/rtmp_protocol/rtmp_define.hpp"

namespace mms {

class RtmpSetPeerBandwidthMessage {
public:
    // 0 - Hard: The peer SHOULD limit its output bandwidth to the
    // indicated window size.
    // 1 - Soft: The peer SHOULD limit its output bandwidth to the the
    // window indicated in this message or the limit already in effect,
    // whichever is smaller.
    // 2 - Dynamic: If the previous Limit Type was Hard, treat this message
    // as though it was marked Hard, otherwise ignore this message.
    #define LIMIE_TYPE_HARD     0
    #define LIMIT_TYPE_SOFT     1
    #define LIMIT_TYPE_DYNAMIC  2
public:
    RtmpSetPeerBandwidthMessage(uint32_t size, uint8_t limit_type) : ack_window_size_(size), limit_type_(limit_type) {

    }

    uint32_t size() {
        return 0;
    }

    int32_t decode(std::shared_ptr<RtmpMessage> rtmp_msg) {
        uint8_t * payload = rtmp_msg->payload_;
        int32_t len = rtmp_msg->payload_size_;
        if (len < 5) {
            return -1;
        }
        // todo modify to ntohl
        ack_window_size_ = ntohl(*(uint32_t*)payload);
        limit_type_ = payload[4];
        return 5;
    }

    std::shared_ptr<RtmpMessage> encode() const {
        std::shared_ptr<RtmpMessage> rtmp_msg = std::make_shared<RtmpMessage>(sizeof(ack_window_size_) + sizeof(limit_type_));
        rtmp_msg->chunk_stream_id_ = RTMP_CHUNK_ID_PROTOCOL_CONTROL_MESSAGE;
        rtmp_msg->timestamp_ = 0;
        rtmp_msg->message_type_id_ = RTMP_MESSAGE_TYPE_SET_PEER_BANDWIDTH;
        rtmp_msg->message_stream_id_ = RTMP_MESSAGE_ID_PROTOCOL_CONTROL;
        // window ack_size
        uint8_t * payload = rtmp_msg->payload_;
        *(uint32_t*)payload = htonl(ack_window_size_);
        // limit type
        payload[4] = limit_type_;
        rtmp_msg->payload_size_ = sizeof(ack_window_size_) + sizeof(limit_type_);

        return rtmp_msg;
    }
private:
    uint32_t ack_window_size_ = 0;
    uint8_t limit_type_;
};
};