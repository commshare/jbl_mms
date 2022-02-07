#pragma once
#include <string>
#include <unordered_map>

#include <boost/algorithm/string.hpp>

namespace mms {
// http method http1.1-rfc2616.txt @5.1.1
#define HTTP_METHOD_OPTIONS     "OPTIONS"
#define HTTP_METHOD_GET         "GET"
#define HTTP_METHOD_HEAD        "HEAD"
#define HTTP_METHOD_POST        "POST"
#define HTTP_METHOD_PUT         "PUT"
#define HTTP_METHOD_DELETE      "DELETE"
#define HTTP_METHOD_TRACE       "TRACE"
#define HTTP_METHOD_CONNECT     "CONNECT"

enum HTTP_METHOD {
    CONNECT     = 0,
    GET         = 1,
    HEAD        = 2,
    POST        = 3,
    PUT         = 4,
    DELETE      = 5,
    TRACE       = 6,
    OPTION      = 7,
};

#define HTTP_CRLF               "/r/n"
#define HTTP_VERSION_1_0        "HTTP/1.0"
#define HTTP_VERSION_1_1        "HTTP/1.1"

// enum HTTP_REQ_STATE {
//     HTTP_STATE_INIT             = 0,
//     HTTP_STATE_REQUEST_LINE     = 1,
//     HTTP_STATE_REQUEST_HEADER   = 2,
//     HTTP_STATE_REQUEST_BODY     = 3,
//     HTTP_STATE_REQUEST_DONE     = 4,
//     HTTP_STATE_REQUEST_ERROR    = 5,
// };

class HttpRequest {
public:
    HTTP_METHOD method_;
    std::string path_;
    std::unordered_map<std::string, std::string> params_;
    std::unordered_map<std::string, std::string> headers_;
    std::string version_;

    std::string body_;
public:
    HTTP_METHOD method() const {
        return method_;
    }
    
    const std::string & getPath() const {
        return path_;
    }

    bool parseRequestLine(const char *buf, size_t len) {
        std::string s(buf, len);
        std::vector<std::string> vs;
        boost::split(vs, s, boost::is_any_of(" "));
        if (vs.size() != 3) {
            return false;
        }
        // get method
        const std::string & m = vs[0];
        if (m == "GET") {
            method_ = GET;
        } else if (m == "POST") {
            method_ = POST;
        } else if (m == "HEAD") {
            method_ = HEAD;
        } else if (m == "OPTION") {
            method_ = OPTION;
        } else if (m == "PUT") {
            method_ = PUT;
        } else if (m == "DELETE") {
            method_ = DELETE;
        } else {
            return false;
        }
        //get path
        const std::string & p = vs[1];
        auto pos = p.find("?");
        if (pos != std::string::npos) {
            path_ = p.substr(0, pos);
            std::string params = p.substr(pos+1);
            std::vector<std::string> vsp;
            boost::split(vsp, params, boost::is_any_of("&"));
            for(auto & ps : vsp) {
                std::vector<std::string> tmp;
                boost::split(tmp, ps, boost::is_any_of("="));
                if (tmp.size() > 1) {
                    if (tmp.size() >= 2) {
                        params_[tmp[0]] = tmp[1];
                    } else {
                        params_[tmp[0]] = "";
                    }
                }
            }
        } else {
            path_ = p;
        }
        // HTTP/1.1
        const std::string & h = vs[2];
        std::vector<std::string> hv;
        boost::split(hv, h, boost::is_any_of("/"));
        if (hv.size() < 2) {
            return false;
        }

        if (hv[0] != "HTTP") {
            return false;
        }

        return true;
    }

    bool parseHeader(const char *buf, size_t len) {
        std::string s(buf, len);
        std::vector<std::string> vs;
        boost::split(vs, s, boost::is_any_of(":"));
        if (vs.size() != 2) {
            return false;
        }

        const std::string & attr = vs[0];
        boost::algorithm::trim(vs[1]);
        const std::string & val = vs[1];
        headers_[attr] = val;
        return true;
    }
};


};