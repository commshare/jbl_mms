#pragma once
#include "http_define.hpp"

namespace mms {
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

    const std::string & getHeader(const std::string & k) const {
        static std::string empty_str("");
        auto it = headers_.find(k);
        if (it == headers_.end()) {
            return empty_str;
        }
        return it->second;
    }

    const std::string & getParam(const std::string & k) const {
        static std::string empty_str("");
        auto it = params_.find(k);
        if (it == params_.end()) {
            return empty_str;
        }
        return it->second;
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