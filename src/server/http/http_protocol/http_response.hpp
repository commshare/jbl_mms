#pragma once
#include <unordered_map>
#include <string>
#include <boost/asio/spawn.hpp>

namespace mms {
class HttpConn;
class HttpResponse {
public:
    HttpResponse(HttpConn *conn) : conn_(conn) {

    }

    void addHeader(const std::string & k, const std::string & v) {
        headers_[k] = v;
    }

    bool writeHeader(int code, boost::asio::yield_context &yield) {
        std::ostringstream ss;
        ss  << "HTTP/1.1 " << code << " OK\r\n";
        for(auto & h : headers_) {
            ss << h.first << ": " << h.second << "\r\n";
        }
        ss << "Access-Control-Allow-Origin: *\r\n";
        ss << "\r\n";
        const std::string & header = ss.str();
        if (!conn_->send((const uint8_t*)header.c_str(), header.size(), yield)) {
            return false;
        }
        return true;
    }

    bool writeData(const uint8_t *data, size_t len, boost::asio::yield_context &yield) {
        if (!conn_->send(data, len, yield)) {
            return false;
        }
        return true;
    }
private:
    HttpConn *conn_;
    std::unordered_map<std::string, std::string> headers_;
};
};