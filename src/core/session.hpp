#pragma once
namespace mms {
class Session {
public:
    virtual void close() = 0;
    std::string & getSessionName() {
        return session_name_;
    }

    void setSessionName(const std::string & session_name) {
        session_name_ = session_name;
    }
protected:
    std::string session_name_;
};

};