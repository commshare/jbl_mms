#pragma once
#include <string>
// 5.3.  Session Name ("s=")
// s=<session name>
namespace mms {
struct SessionName {
public:
    static std::string prefix;
    bool parse(const std::string & line);

    const std::string & getSessionName() const {
        return session_name;
    }

    void setSessionName(const std::string & val) {
        session_name = val;
    }

    std::string toString() const;
public:
    //    The "s=" field is the textual session name.  There MUST be one and
    //    only one "s=" field per session description.  The "s=" field MUST NOT
    //    be empty and SHOULD contain ISO 10646 characters (but see also the
    //    "a=charset" attribute).  If a session has no meaningful name, the
    //    value "s= " SHOULD be used (i.e., a single space as the session
    //    name).
    std::string session_name;
};
};