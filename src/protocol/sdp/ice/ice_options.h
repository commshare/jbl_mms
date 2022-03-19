#pragma once
#include <string_view>
// 15.5.  "ice-options" Attribute

//    The "ice-options" attribute is a session-level attribute.  It
//    contains a series of tokens that identify the options supported by
//    the agent.  Its grammar is:

//    ice-options           = "ice-options" ":" ice-option-tag
//                              0*(SP ice-option-tag)
//    ice-option-tag        = 1*ice-char
namespace mms {
struct IceOption {
public:
    static std::string_view prefix;
    bool parse(const std::string_view & line);
public:
    std::string_view raw_string;
    std::string_view valid_string;
    std::string_view option;
};
};