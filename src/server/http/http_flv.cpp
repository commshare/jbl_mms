#include "http_flv.hpp"
#include "protocol/flv/flv_define.hpp"
#include "server/http/http_protocol/http_response.hpp"
#include "core/media_manager.hpp"

using namespace mms;

HttpFlv::HttpFlv(HttpSession *http_session, std::shared_ptr<HttpRequest> http_req, std::shared_ptr<HttpResponse> http_resp) : RtmpMediaSink(http_session->getWorker()), http_session_(http_session) {
    http_request_ = http_req;
    http_response_ = http_resp;
    conn_ = http_session->conn_;
    send_buf_.resize(1024);
    send_buf_size_ = 1024;
}

boost::asio::awaitable<bool> HttpFlv::sendRtmpMessage(std::shared_ptr<RtmpMessage> pkt) {
    sending_rtmp_msgs_.emplace_back(pkt);
    co_return true;
    // send_handler_();
}

bool HttpFlv::startSendRtmpMessage() {
    send_handler_();
    return true;
}

void HttpFlv::close() {
    // todo: how to record 404 error to log.
    if (closed_) {
        return;
    }

    closed_ = true;
    auto source = MediaManager::get_mutable_instance().getSource(http_session_->getSessionName());
    if (source) {
        source->removeMediaSink(std::dynamic_pointer_cast<MediaSink>(shared_from_this()));
    }
    conn_->close();
}

bool HttpFlv::init() {
    send_handler_ = [this]() {
        if (sending_rtmp_msgs_.empty() || !send_header_done_) {
            return;
        }

        if (sending_) {
            return;
        }
        sending_ = true;
        boost::asio::co_spawn(conn_->getWorker()->getIOContext(), [this]()->boost::asio::awaitable<void> {
            if (!sending_rtmp_msgs_.empty()) {
                std::list<std::shared_ptr<mms::RtmpMessage>> msgs;
                sending_rtmp_msgs_.swap(msgs);
                
                for (auto it = msgs.begin(); it != msgs.end(); it++) {
                    uint32_t d = htonl(prev_tag_size_);
                    if (!(co_await conn_->send((uint8_t*)&d, 4))) {
                        conn_->close();
                        co_return;
                    }

                    int32_t header_size = FlvTagHeader::encodeFromRtmpMessage(*it, (uint8_t*)send_buf_.data(), send_buf_size_);
                    if (!(co_await conn_->send((uint8_t*)send_buf_.data(), header_size))) {
                        conn_->close();
                        co_return;
                    }
                    if (!(co_await conn_->send((*it)->payload_, (*it)->payload_size_))) {
                        conn_->close();
                        co_return;
                    }
                    
                    prev_tag_size_ = (*it)->payload_size_ + header_size;
                }
            }
            sending_ = false;
        }, boost::asio::detached);
    };
    
    boost::asio::co_spawn(worker_->getIOContext(), [this]()->boost::asio::awaitable<void> {
        http_response_->addHeader("Content-Type", "video/x-flv");
        http_response_->addHeader("Connection", "Keep-Alive");
        if (!(co_await http_response_->writeHeader(200))) {
            conn_->close();
            co_return;
        }

        auto consumed = FlvHeader::encode((uint8_t*)send_buf_.data(), send_buf_size_);
        if (!(co_await http_response_->writeData((uint8_t*)send_buf_.data(), consumed))) {
            conn_->close();
            co_return;
        }

        send_header_done_ = true;
        send_handler_();
    }, boost::asio::detached);
    return true;
}