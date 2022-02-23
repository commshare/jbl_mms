#pragma once
#include <string>
#include <optional>
#include <vector>
// Media description, if present
//     m=  (media name and transport address)
//     i=* (media title)
//     c=* (connection information -- optional if included at
//         session level)
//     b=* (zero or more bandwidth information lines)
//     k=* (encryption key)
//     a=* (zero or more media attribute lines)
// Some attributes (the ones listed in Section 6 of this memo)
//    have a defined meaning, but others may be added on an application-,
//    media-, or session-specific basis.  An SDP parser MUST ignore any
//    attribute it doesn't understand.
namespace mms {
struct MediaSdp {
public:


public:
    std::string media_info;
    std::optional<std::string> media_title;
    std::optional<std::string> connection_information;
    std::vector<std::string> bandwidth_information;
    std::optional<std::string> encryption_key;
    std::vector<std::string> media_attrs;
};
};