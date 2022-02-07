#pragma once
#include "server/http/http_protocol/http_define.hpp"
#include "http_session.hpp"

namespace mms {
class HttpHandler {
public:
    bool virtual onHttpRequest(HttpSession & http_session, const HttpRequest & http_req) = 0;
};

};