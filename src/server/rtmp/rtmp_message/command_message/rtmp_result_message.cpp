#include "rtmp_result_message.hpp"
using namespace mms;
RtmpResultMessage::RtmpResultMessage(const std::string & name) {
    command_name_.setValue(name);
}

RtmpResultMessage::~RtmpResultMessage() {

}

int32_t RtmpResultMessage::decode(std::shared_ptr<RtmpMessage> rtmp_msg) {
    return 0;
}

std::shared_ptr<RtmpMessage> RtmpResultMessage::encode() {
    // todo implement this method
    return nullptr;
}
