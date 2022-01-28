#pragma once
#include <memory>
#include <vector>

#include "server/rtmp/rtmp_conn/rtmp_conn.hpp"
#include "server/rtmp/rtmp_protocol/rtmp_handshake.hpp"
#include "server/rtmp/rtmp_protocol/rtmp_chunk_protocol.hpp"

namespace mms {
template<typename PKT>
class MediaSource : public MediaStream<PKT> {
public:
    MediaSource() {
        
    }

    virtual ~MediaSource() {

    }
};

};