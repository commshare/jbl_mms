#pragma once

namespace mms {
enum RtmpMessageType {
    E_RtmpConnectCommandMessage = 0,
};

class RtmpCommonMessage {
public:
    RtmpMessageType getMessageType() {
        return message_type_;
    }
protected:
    RtmpMessageType message_type_;
};

};