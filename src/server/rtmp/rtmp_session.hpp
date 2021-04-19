#pragma once
#include <memory>

#include "rtmp_conn.hpp"
#include "rtmp_handshake.hpp"

namespace mms {
class RtmpSession {
public:
    RtmpSession(std::unique_ptr<RtmpConn> conn) {
        conn_ = std::move(conn);
    }

    virtual ~RtmpSession() {

    }

    void start() {

    }
private:
    std::unique_ptr<RtmpConn> conn_;
    RtmpHandshake handshake_;
};

};