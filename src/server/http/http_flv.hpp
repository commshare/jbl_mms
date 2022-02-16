#pragma once
#include <memory>
#include <list>
#include <atomic>

#include "core/rtmp_media_sink.hpp"
#include "http_session.hpp"
#include "server/http/http_protocol/http_response.hpp"

namespace mms {
class HttpFlv : public RtmpMediaSink, public std::enable_shared_from_this<HttpFlv> {
public:
    HttpFlv(HttpSession *http_session, std::shared_ptr<HttpRequest> http_request, std::shared_ptr<HttpResponse> http_response);
    bool init();
    void close();
private:
    bool sendRtmpMessage(std::shared_ptr<RtmpMessage> pkt) override;
    bool startSendRtmpMessage() override;
    HttpSession *http_session_;
    std::shared_ptr<HttpRequest> http_request_;
    std::shared_ptr<HttpResponse> http_response_;

    std::list<std::shared_ptr<RtmpMessage>> sending_rtmp_msgs_;
    std::atomic<bool> sending_{false};
    std::function<void()> send_handler_;
    HttpConn *conn_;
    uint32_t prev_tag_size_ = 0;
    std::string send_buf_;
    size_t send_buf_size_ = 0;
    bool send_header_done_ = false;
    std::atomic<bool> closed_{false};
};

};