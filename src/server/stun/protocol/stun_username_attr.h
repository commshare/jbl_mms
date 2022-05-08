/*
@https://blog.csdn.net/u012538729/article/details/115694308
USERNAME：用户名，用于消息完整性，在webrtc中的规则为 “对端的ice-ufrag：自己的ice-ufrag”，其中ice-ufrag已通过提议/应答的SDP信息进行交互。


11.2.6 USERNAME

   The USERNAME attribute is used for message integrity.  It serves as a
   means to identify the shared secret used in the message integrity
   check.  The USERNAME is always present in a Shared Secret Response,
   along with the PASSWORD.  It is optionally present in a Binding
   Request when message integrity is used.





Rosenberg, et al.           Standards Track                    [Page 28]

RFC 3489                          STUN                        March 2003


   The value of USERNAME is a variable length opaque value.  Its length
   MUST be a multiple of 4 (measured in bytes) in order to guarantee
   alignment of attributes on word boundaries.
*/

#pragma once
#include <string>
#include "stun_define.hpp"
namespace mms {
struct StunUsernameAttr : public StunMsgAttr
{
    StunUsernameAttr(const std::string & username) : StunMsgAttr(STUN_ATTR_USERNAME), user_name_(username)
    {
        
    }

    StunUsernameAttr() = default;

    size_t size();

    int32_t encode(uint8_t *data, size_t len);

    int32_t decode(uint8_t *data, size_t len);

    const std::string & getUserName() const {
        return user_name_;
    }

    void setUserName(const std::string & username) {
        user_name_ = username;
    }
private:
    std::string user_name_;
};
};