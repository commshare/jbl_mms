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
class RtmpReleaseStreamRespMessage {
public:
    RtmpReleaseStreamRespMessage(const RtmpReleaseStreamMessage & rel_msg, const std::string & name) {
        command_name_.setValue(name);
        transaction_id_.setValue(rel_msg.transaction_id_.getValue());
    }

    virtual ~RtmpReleaseStreamRespMessage() {

    }
public:
    int32_t decode(std::shared_ptr<RtmpMessage> rtmp_msg) {
        return 0;
    }

    std::shared_ptr<RtmpMessage> encode() const {
        size_t s = 0;
        s += command_name_.size();
        s += transaction_id_.size();
        s += command_obj_.size();
        s += udef_.size();

        std::shared_ptr<RtmpMessage> rtmp_msg = std::make_shared<RtmpMessage>(s);
        rtmp_msg->chunk_stream_id_ = RTMP_CHUNK_ID_PROTOCOL_CONTROL_MESSAGE;
        rtmp_msg->timestamp_ = 0;
        rtmp_msg->message_type_id_ = RTMP_MESSAGE_TYPE_AMF0_COMMAND;
        rtmp_msg->message_stream_id_ = RTMP_MESSAGE_ID_PROTOCOL_CONTROL;
        // window ack_size
        uint8_t * payload = rtmp_msg->payload_;
        int32_t len = s;
        int32_t consumed = command_name_.encode(payload, len);
        if (consumed < 0) {
            return nullptr;
        }
        payload += consumed;
        len -= consumed;

        consumed = transaction_id_.encode(payload, len);
        if (consumed < 0) {
            return nullptr;
        }
        payload += consumed;
        len -= consumed;

        consumed = command_obj_.encode(payload, len);
        if (consumed < 0) {
            return nullptr;
        }
        payload += consumed;
        len -= consumed;

        consumed = udef_.encode(payload, len);
        if (consumed < 0) {
            return nullptr;
        }
        payload += consumed;
        len -= consumed;
        rtmp_msg->payload_size_ = s;
        return rtmp_msg;
    }

    Amf0Object & cmdObj() {
        return command_obj_;
    }

private:
    Amf0String command_name_;
    Amf0Number transaction_id_;
    Amf0Object command_obj_;
    Amf0Undefined udef_;
};
};