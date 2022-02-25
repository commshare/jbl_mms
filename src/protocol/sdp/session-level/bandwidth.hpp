#pragma once
#include <string_view>
namespace mms {
// 5.8.  Bandwidth ("b=")
//       b=<bwtype>:<bandwidth>
// This OPTIONAL field denotes the proposed bandwidth to be used by the
//    session or media.  The <bwtype> is an alphanumeric modifier giving
//    the meaning of the <bandwidth> figure.  Two values are defined in
//    this specification, but other values MAY be registered in the future
//    (see Section 8 and [21], [25]):
struct BandWidth {
public:
    std::string_view raw_string;
    std::string_view bw_type;
    std::string_view bandwidth;   
};
};