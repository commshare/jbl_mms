#include <iostream>
#include "rtmp_conn.hpp"
#include "../rtmp_session.hpp"
using namespace mms;

std::shared_ptr<RtmpSession> RtmpConn::createSession() {
    session_ = std::make_shared<RtmpSession>(this);
    return session_;
}

std::shared_ptr<RtmpSession> RtmpConn::getSession() {
    return session_;
}