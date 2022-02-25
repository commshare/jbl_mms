#pragma once
#include <stdint.h>
#include <string>
#include <optional>
#include <vector>
#include <string_view>
// Session description
// v=  (protocol version)
// o=  (originator and session identifier)
// s=  (session name)
// i=* (session information)
// u=* (URI of description)
// e=* (email address)
// p=* (phone number)
// c=* (connection information -- not required if included in
//     all media)
// b=* (zero or more bandwidth information lines)
// One or more time descriptions ("t=" and "r=" lines; see below)
// z=* (time zone adjustments)
// k=* (encryption key)
// a=* (zero or more session attribute lines)
// Zero or more media descriptions
// Time description
// t=  (time the session is active)
// r=* (zero or more repeat times)
namespace mms {
struct SessionDescription {
public:
    int32_t read(uint8_t *data, size_t len);
public:
    std::string protocol_version;
    std::string session_identifier;
    std::string session_name;
    std::optional<std::string> session_information;
    std::optional<std::string> uri;
    std::optional<std::string> email_address;
    std::optional<std::string> phone_number;
    std::optional<std::string> connection_information;
    std::vector<std::string> bandwidth_information;
    std::optional<std::string> time_zone;
    std::optional<std::string> encrtption_key;
    std::vector<std::string> session_attr;
    std::string time_session;
    std::vector<std::string> repeat_times;
};
};