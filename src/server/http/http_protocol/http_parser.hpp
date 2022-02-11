#pragma once
#include <memory>
#include <functional>

#include "http_define.hpp"
#include "http_request.hpp"

namespace mms {
enum HTTP_STATE {
    HTTP_STATE_WAIT_REQUEST_LINE    =   0,
    HTTP_STATE_WAIT_HEADER          =   1,
    HTTP_STATE_REQUEST_ERROR        =   2,
};

class HttpParser {
public:
    int32_t read(const std::string & buf) {
        switch(state_) {
            case HTTP_STATE_WAIT_REQUEST_LINE: {
                http_req_ = std::make_shared<HttpRequest>();
                auto pos = buf.find("\r\n");
                if (pos != std::string::npos) {
                    if (!http_req_->parseRequestLine(buf.data(), pos)) {
                        state_ = HTTP_STATE_REQUEST_ERROR;
                        return -1;
                    } else {
                        state_ = HTTP_STATE_WAIT_HEADER;
                        return pos + 2;
                    }
                } else {
                    return 0;
                }
                break;
            }
            case HTTP_STATE_WAIT_HEADER: {
                auto pos = buf.find("\r\n");
                if (pos != std::string::npos) {
                    if (pos == 0) {
                        // content-len 得到长度
                        if (http_req_->method() == GET) {
                            req_cb_(http_req_);
                        }
                    } else {
                        if (!http_req_->parseHeader(buf.data(), pos)) {
                            state_ = HTTP_STATE_REQUEST_ERROR;
                            return -2;
                        } else {
                            state_ = HTTP_STATE_WAIT_HEADER;
                            return pos + 2;
                        }
                    }
                }
                break;
            }
            // case HTTP_STATE_REQUEST_BODY: {
            // }
        }
        return 0;
    }

    void onHttpRequest(const std::function<void(std::shared_ptr<HttpRequest>)> & cb) {
        req_cb_ = cb;
    }
private:
    std::shared_ptr<HttpRequest> http_req_;
    std::function<void(std::shared_ptr<HttpRequest>)> req_cb_;
    HTTP_STATE state_ = HTTP_STATE_WAIT_REQUEST_LINE;
};

};