#include "http_response.hpp"
#include "../http_conn.hpp"
using namespace mms;

HttpResponse::HttpResponse(HttpConn *conn) : conn_(conn) {
}

void HttpResponse::addHeader(const std::string & k, const std::string & v) {
    headers_[k] = v;
}

boost::asio::awaitable<bool> HttpResponse::writeHeader(int code) {
    std::ostringstream ss;
    ss  << "HTTP/1.1 " << code << " OK\r\n";
    for(auto & h : headers_) {
        ss << h.first << ": " << h.second << "\r\n";
    }
    ss << "Access-Control-Allow-Origin: *\r\n";
    ss << "\r\n";
    const std::string & header = ss.str();
    if (!(co_await conn_->send((const uint8_t*)header.c_str(), header.size()))) {
        co_return false;
    }
    co_return true;
}

boost::asio::awaitable<bool> HttpResponse::writeData(const uint8_t *data, size_t len) {
    if (!(co_await conn_->send(data, len))) {
        co_return false;
    }
    co_return true;
}