/*
MIT License

Copyright (c) 2021 jiangbaolin

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#pragma once
#include <string>
#include "server/rtmp/amf0/amf0_inc.hpp"
#include "server/rtmp/rtmp_protocol/rtmp_define.hpp"

namespace mms {
class RtmpSetChunkSizeMessage {
public:
    RtmpSetChunkSizeMessage(size_t s) : chunk_size_(s) {

    }

    RtmpSetChunkSizeMessage() {

    }

    int32_t decode(std::shared_ptr<RtmpMessage> rtmp_msg) {
        uint8_t * payload = rtmp_msg->payload_;
        int32_t len = rtmp_msg->payload_size_;
        if (len < 4) {
            return -1;
        }
        chunk_size_ = ntohl(*(uint32_t*)payload);
        return 4;
    }

    std::shared_ptr<RtmpMessage> encode() const {
        std::shared_ptr<RtmpMessage> rtmp_msg = std::make_shared<RtmpMessage>(sizeof(chunk_size_));
        rtmp_msg->chunk_stream_id_ = RTMP_CHUNK_ID_PROTOCOL_CONTROL_MESSAGE;
        rtmp_msg->timestamp_ = 0;
        rtmp_msg->message_type_id_ = RTMP_MESSAGE_TYPE_SET_CHUNK_SIZE;
        rtmp_msg->message_stream_id_ = RTMP_MESSAGE_ID_PROTOCOL_CONTROL;
        // chunk_size_
        *(uint32_t*)rtmp_msg->payload_ = htonl(chunk_size_);
        rtmp_msg->payload_size_ = sizeof(chunk_size_);
        return rtmp_msg;
    }
public:
    int32_t chunk_size_;
};
};