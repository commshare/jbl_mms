#pragma once
#include <string_view>
// 5.2.  Origin ("o=")
namespace mms {
struct Origin {
public:
    static std::string_view prefix;
    bool parse(const std::string_view & line);
public:
    std::string_view raw_string;// 原始字符串
    std::string_view valid_string;//去除\r\n后的字符串
    // <username> is the user's login on the originating host, or it is "-"
    //   if the originating host does not support the concept of user IDs.
    //   The <username> MUST NOT contain spaces.
    std::string_view username;
    // <sess-id> is a numeric string such that the tuple of <username>,
    //   <sess-id>, <nettype>, <addrtype>, and <unicast-address> forms a
    //   globally unique identifier for the session.
    std::string_view session_id;
    // <sess-version> is a version number for this session description.
    std::string_view session_version;
    // <nettype> is a text string giving the type of network.  Initially
    //   "IN" is defined to have the meaning "Internet", but other values
    //   MAY be registered in the future
    std::string_view nettype;
    // <addrtype> is a text string giving the type of the address that
    //   follows.  Initially "IP4" and "IP6" are defined, but other values
    //   MAY be registered in the future (see Section 8).
    std::string_view addrtype;
    // <unicast-address> is the address of the machine from which the
    //   session was created.  For an address type of IP4, this is either
    //   the fully qualified domain name of the machine or the dotted-
    //   decimal representation of the IP version 4 address of the machine.
    //   For an address type of IP6, this is either the fully qualified
    //   domain name of the machine or the compressed textual
    //   representation of the IP version 6 address of the machine.  For
    //   both IP4 and IP6, the fully qualified domain name is the form that
    //   SHOULD be given unless this is unavailable, in which case the
    //   globally unique address MAY be substituted.
    //   For privacy reasons, it is sometimes desirable to obfuscate(故意混淆) the
    //   username and IP address of the session originator.  If this is a
    //   concern, an arbitrary <username> and private <unicast-address> MAY be
    //   chosen to populate the "o=" field, provided that these are selected
    //   in a manner that does not affect the global uniqueness of the field.
    std::string_view unicast_address;
    
};
};