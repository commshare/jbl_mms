#pragma once
namespace mms {
class Session {
public:
    virtual void close() = 0;
    std::string & getSessionName() {
        return session_name_;
    }
protected:
    std::string session_name_;
};

};