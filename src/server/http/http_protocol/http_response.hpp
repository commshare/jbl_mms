#pragma once
#include <unordered_map>
#include <string>
#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp>

namespace mms {
class HttpConn;
class HttpResponse {
public:
    HttpResponse(HttpConn *conn);
    void addHeader(const std::string & k, const std::string & v);
    boost::asio::awaitable<bool> writeHeader(int code);
    boost::asio::awaitable<bool> writeData(const uint8_t *data, size_t len);
private:
    HttpConn *conn_;
    std::unordered_map<std::string, std::string> headers_;
};
};