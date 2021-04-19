#pragma once

namespace mms {
class RtmpMessage {
public:
    char *payload_;
    int32_t curr_size_;
    int32_t payload_len_;
};

class ChunkMessageHeader {
public:
    int32_t timestamp_;
    int32_t message_length_;
    uint8_t message_type_id_;
    int32_t message_stream_id_;
};

class RtmpChunk {
public:
    ChunkMessageHeader chunk_message_header_;
public:
    RtmpMessage *rtmp_message_ = nullptr;
};

};