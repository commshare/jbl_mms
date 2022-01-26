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
#include "rtmp_result_message.hpp"
namespace mms {
class RtmpConnectRespMessage : public RtmpResultMessage {
public:
    RtmpConnectRespMessage(const RtmpConnectCommandMessage & conn_msg, const std::string & name):RtmpResultMessage(name) {
        transaction_id_.setValue(conn_msg.transaction_id_.getValue());
    }

    virtual ~RtmpConnectRespMessage() {

    }
public:
    int32_t decode(std::shared_ptr<RtmpMessage> rtmp_msg) {
        return 0;
    }

    std::shared_ptr<RtmpMessage> encode() {
        return nullptr;
    }
};
};