#pragma once
#include <string>
#include <string>
//   a=tool:<name and version of tool>

//      This gives the name and version number of the tool used to
//      create the session description.  It is a session-level
//      attribute, and it is not dependent on charset.
namespace mms {
struct ToolAttr {
public:
    static std::string prefix;

    ToolAttr() = default;
    ToolAttr(const std::string & name) {
        name_ = name;
    }

    bool parse(const std::string & line);

    const std::string & getName() const {
        return name_;
    }

    void setName(const std::string & val) {
        name_ = val;
    }

    std::string toString() const;
private:
    std::string name_;
};
};