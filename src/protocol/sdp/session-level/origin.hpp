#pragma once
#include <string>
// 5.2.  Origin ("o=")
namespace mms {
struct Origin {
public:
    static std::string prefix;
    bool parse(const std::string & line);

    inline const std::string & getUserName() const {
        return username;
    }

    inline void setUserName(const std::string & u) {
        username = u;
    }

    inline uint64_t getSessionId() {
        return session_id;
    }

    void setSessionId(uint64_t sid) {
        session_id = sid;
    }

    uint32_t getSessionVersion() {
        return session_version;
    }

    void setSessionVersion(uint32_t v) {
        session_version = v;
    }

    const std::string & getNetType() const {
        return nettype;
    }

    void setNetType(const std::string & nt) {
        nettype = nt;
    }

    const std::string & getAddrType() const {
        return addrtype;
    }

    void setAddrType(const std::string & at) {
        addrtype = at;
    }

    const std::string & getUnicastAddress() const {
        return unicast_address;
    }

    void setUnicastAddress(const std::string & ua) {
        unicast_address = ua;
    }

    std::string toString() const;
private:
    // <username> is the user's login on the originating host, or it is "-"
    //   if the originating host does not support the concept of user IDs.
    //   The <username> MUST NOT contain spaces.
    std::string username;
    // <sess-id> is a numeric string such that the tuple of <username>,
    //   <sess-id>, <nettype>, <addrtype>, and <unicast-address> forms a
    //   globally unique identifier for the session.
    uint64_t session_id;
    // <sess-version> is a version number for this session description.
    uint32_t session_version;
    // <nettype> is a text string giving the type of network.  Initially
    //   "IN" is defined to have the meaning "Internet", but other values
    //   MAY be registered in the future
    std::string nettype;
    // <addrtype> is a text string giving the type of the address that
    //   follows.  Initially "IP4" and "IP6" are defined, but other values
    //   MAY be registered in the future (see Section 8).
    std::string addrtype;
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
    std::string unicast_address;
    
};
};