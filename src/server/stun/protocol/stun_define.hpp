#pragma once
#include <vector>
namespace mms {
#define STUN_BINDING_REQUEST                0x0001
#define STUN_BINDING_RESPONSE               0x0101
#define STUN_BINDING_ERROR_RESPONSE         0x0111
#define STUN_SHARED_SECRET_REQUEST          0x0002
#define STUN_SHARED_SECRET_RESPONSE         0x0102
#define STUN_SHARED_SECRET_ERROR_RESPONSE   0x0112

struct StunMsgHeader {
    uint16_t type;
    uint16_t length;
    uint8_t transaction_id[16];
};

#define STUN_ATTR_MAPPED_ADDRESS            0x0001
#define STUN_ATTR_RESPONSE_ADDRESS          0x0002
#define STUN_ATTR_CHANGE_REQUEST            0x0003
#define STUN_ATTR_SOURCE_ADDRESS            0x0004
#define STUN_ATTR_CHANGED_ADDRESS           0x0005
#define STUN_ATTR_USERNAME                  0x0006
#define STUN_ATTR_PASSWORD                  0x0007
#define STUN_ATTR_MESSAGE_INTEGRITY         0x0008
#define STUN_ATTR_ERROR_CODE                0x0009
#define STUN_ATTR_UNKNOWN_ATTRIBUTES        0x000a
#define STUN_ATTR_REFLECTED_FROM            0x000b

struct StunMsgAttr {
public:
    uint16_t type;
    uint16_t length;
    uint8_t *value;
};

struct StunMsg {
    StunMsgHeader header;
    std::vector<StunMsgAttr*> attrs;
};

//     0                   1                   2                   3
//     0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//    |x x x x x x x x|    Family     |           Port                |
//    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//    |                             Address                           |
//    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct StunMappedAddressAttr : public StunMsgAttr {
    StunMappedAddressAttr() : type(STUN_ATTR_MAPPED_ADDRESS) {

    }

    uint8_t family; // always 0x01
    uint16_t port;
    uint32_t address;
};

struct StunResponseAddressAttr : public StunMsgAttr {
    StunResponseAddressAttr() : type(STUN_ATTR_RESPONSE_ADDRESS) {

    }

    uint8_t family; // always 0x01
    uint16_t port;
    uint32_t address;
};

struct StunChangeAddressAttr : public StunMsgAttr {
    StunChangeAddressAttr() : type(STUN_ATTR_CHANGED_ADDRESS) {

    }

    uint8_t family; // always 0x01
    uint16_t port;
    uint32_t address;
};

struct StunChangeRequestAttr : public StunMsgAttr {
    StunChangeRequestAttr() : type(STUN_ATTR_CHANGE_REQUEST) {

    }

    bool change_ip = false;
    bool change_port = false;
};

struct StunSourceAddressAttr : public StunMsgAttr {
    StunChangeRequestAttr() : type(STUN_ATTR_SOURCE_ADDRESS) {

    }

    uint8_t family; // always 0x01
    uint16_t port;
    uint32_t address;
};

struct StunUsernameAttr : public StunMsgAttr {
    StunUsernameAttr() : type(STUN_ATTR_SOURCE_ADDRESS) {

    }

    std::string user_name;
};

struct StunPasswordAttr : public StunMsgAttr {
    StunPasswordAttr() : type(STUN_ATTR_PASSWORD) {

    }

    std::string password;
};

struct StunMessageIntegrityAttr : public StunMsgAttr {
    StunMessageIntegrityAttr() : type(STUN_ATTR_MESSAGE_INTEGRITY) {

    }

    std::string hmac_sha1;
};

struct StunErrorCodeAttr : public StunMsgAttr {
    StunErrorCodeAttr() : type(STUN_ATTR_ERROR_CODE) {

    }

    uint8_t _class;
    uint8_t number;
    uint32_t reason;
};

struct StunUnknownAttributesAttr : public StunMsgAttr {
    StunUnknownAttributesAttr() : type(STUN_ATTR_UNKNOWN_ATTRIBUTES) {

    }

    uint16_t attr[4];
};


struct StunReflectFromAttr : public StunMsgAttr {
    StunReflectFromAttr() : type(STUN_ATTR_REFLECTED_FROM) {

    }

    uint8_t family; // always 0x01
    uint16_t port;
    uint32_t address;
};

};

