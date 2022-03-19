#pragma once
#include <string_view>
#include <string>
namespace mms {
// 5.4.  Session Information ("i=")
//     i=<session description>
// The "i=" field provides textual information about the session.  There
//    MUST be at most one session-level "i=" field per session description,
//    and at most one "i=" field per media.  If the "a=charset" attribute
//    is present, it specifies the character set used in the "i=" field.
//    If the "a=charset" attribute is not present, the "i=" field MUST
//    contain ISO 10646 characters in UTF-8 encoding.

//    A single "i=" field MAY also be used for each media definition.  In
//    media definitions, "i=" fields are primarily intended for labelling
//    media streams.  As such, they are most likely to be useful when a
//    single session has more than one distinct media stream of the same
//    media type.  An example would be two different whiteboards, one for
//    slides and one for feedback and questions.

//    The "i=" field is intended to provide a free-form human-readable
//    description of the session or the purpose of a media stream.  It is
//    not suitable for parsing by automata.
struct SessionInformation {
public:
    static std::string_view prefix;
    bool parse(const std::string_view & line);
public:
    std::string_view raw_string;
    std::string_view valid_string;
    std::string_view session_information;
};
};