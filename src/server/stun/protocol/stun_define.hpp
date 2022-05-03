#pragma once
#include <vector>
#include <memory>
#include <string>
#include <netinet/in.h>
#include <string.h>

namespace mms {
#define STUN_BINDING_REQUEST                0x0001
#define STUN_BINDING_RESPONSE               0x0101
#define STUN_BINDING_ERROR_RESPONSE         0x0111
#define STUN_SHARED_SECRET_REQUEST          0x0002
#define STUN_SHARED_SECRET_RESPONSE         0x0102
#define STUN_SHARED_SECRET_ERROR_RESPONSE   0x0112

struct StunMsg;

struct StunMsgHeader {
    uint16_t type;
    uint16_t length;
    uint8_t transaction_id[16];
    int32_t decode(uint8_t *data, size_t len) {
        if (len < 20) {
            return -1;
        }

        uint8_t *data_start = data;
        type = ntohs(*(uint16_t*)data);
        data += 2;
        len = ntohs(*(uint16_t*)data);
        data += 2;
        memcpy(transaction_id, data, 16);
        return 20;
    }

    int32_t encode(uint8_t *data, size_t len) {
        if (len < 20) {
            return -1;
        }
        *(uint16_t*)data = htons(type);
        data += 2;
        *(uint16_t*)data = htons(length);
        data += 2;
        memcpy(data, transaction_id, 16);
        return 20;
    }

    size_t size() {
        return 20;
    }
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
    StunMsgAttr() {

    }

    StunMsgAttr(uint16_t t) : type(t) {

    }

    virtual size_t size() {
        return 4;
    }

    virtual int32_t encode(uint8_t *data, size_t len) {
        if (len < 4) {
            return -1;
        }
        *(uint16_t *)data = htons(type);
        data += 2;
        *(uint16_t *)data = htons(length);
        return 4;
    }

    virtual int32_t decode(uint8_t *data, size_t len) {
        if (len < 4) {
            return -1;
        }
        type = ntohs(*(uint16_t *)data);
        data += 2;
        length = ntohs(*(uint16_t *)data);
        data += 2;
        return 4;
    }

    uint16_t getType() {
        return type;
    }
    
    uint16_t type;
    uint16_t length;
};

struct StunMsg {
    StunMsgHeader header;
    std::vector<std::unique_ptr<StunMsgAttr>> attrs;

    void addAttr(std::unique_ptr<StunMsgAttr> attr) {
        attrs.emplace_back(std::move(attr));
    }

    uint16_t type() {
        return header.type;
    }

    int32_t decode(uint8_t *data, size_t len);

    size_t size() {
        int32_t s = 0;
        for (auto & attr : attrs) {
            s += attr->size();
        }
        s += header.size();
        return s;
    }

    virtual int32_t encode(uint8_t *data, size_t len) {
        int32_t content_len = 0;
        for(auto & attr : attrs) {
            content_len += attr->size();
        }
        header.length = content_len;

        uint8_t *data_start = data;
        int32_t consumed = header.encode(data, len);
        if (consumed < 0) {
            return -1;
        }

        data += consumed;
        len -= consumed;
        for(auto & attr : attrs) {
            consumed = attr->encode(data, len);
            if (consumed < 0) {
                return -2;
            }
            data += consumed;
            len -= consumed;
        }
        return data - data_start;
    }
};

//     0                   1                   2                   3
//     0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//    |x x x x x x x x|    Family     |           Port                |
//    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//    |                             Address                           |
//    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct StunMappedAddressAttr : public StunMsgAttr {
    StunMappedAddressAttr(uint32_t addr, uint16_t p) : StunMsgAttr(STUN_ATTR_MAPPED_ADDRESS), family(0x01) {
        address = addr;
        port = p;
    }

    StunMappedAddressAttr() : family(0x01) {
        
    }

    size_t size() {
        return StunMsgAttr::size() + 8;
    }

    int32_t encode(uint8_t *data, size_t len) {
        length = 8;
        uint8_t *data_start = data;
        int32_t consumed = StunMsgAttr::encode(data, len);
        if (consumed < 0) {
            return -1;
        }
        data += consumed;
        len -= consumed;
        if (len < 8) {
            return -2;
        }

        data[1] = family;
        data[0] = 0;
        data += 2;
        *(uint16_t*)data = htons(port);
        data += 2;
        *(uint32_t*)data = htonl(address);
        data += 4;
        return data - data_start;
    }

    int32_t decode(uint8_t *data, size_t len) {
        uint8_t *data_start = data;
        int32_t consumed = StunMsgAttr::decode(data, len);
        if (consumed < 0) {
            return -1;
        }
        data += consumed;
        len -= consumed;

        if (len < 8) {
            return -2;
        }
        family = data[1];
        data += 2;
        port = ntohs(*(uint16_t*)data);
        data += 2;
        address = ntohl(*(uint32_t*)data);
        data += 4;
        return data - data_start;
    }

    uint8_t family; // always 0x01
    uint16_t port;
    uint32_t address;
};

struct StunResponseAddressAttr : public StunMsgAttr {
    StunResponseAddressAttr() : StunMsgAttr(STUN_ATTR_RESPONSE_ADDRESS) {

    }

    uint8_t family; // always 0x01
    uint16_t port;
    uint32_t address;
};

struct StunChangeAddressAttr : public StunMsgAttr {
    StunChangeAddressAttr() : StunMsgAttr(STUN_ATTR_CHANGED_ADDRESS) {

    }

    uint8_t family; // always 0x01
    uint16_t port;
    uint32_t address;
};

struct StunChangeRequestAttr : public StunMsgAttr {
    StunChangeRequestAttr() : StunMsgAttr(STUN_ATTR_CHANGE_REQUEST) {

    }

    bool change_ip = false;
    bool change_port = false;
};

struct StunSourceAddressAttr : public StunMsgAttr {
    StunSourceAddressAttr() : StunMsgAttr(STUN_ATTR_SOURCE_ADDRESS) {

    }

    uint8_t family; // always 0x01
    uint16_t port;
    uint32_t address;
};

struct StunUsernameAttr : public StunMsgAttr {
    StunUsernameAttr() : StunMsgAttr(STUN_ATTR_SOURCE_ADDRESS) {

    }

    std::string user_name;
};

struct StunPasswordAttr : public StunMsgAttr {
    StunPasswordAttr() : StunMsgAttr(STUN_ATTR_PASSWORD) {

    }

    std::string password;
};

struct StunMessageIntegrityAttr : public StunMsgAttr {
    StunMessageIntegrityAttr() : StunMsgAttr(STUN_ATTR_MESSAGE_INTEGRITY) {

    }

    std::string hmac_sha1;
};

struct StunErrorCodeAttr : public StunMsgAttr {
    StunErrorCodeAttr(int32_t code, const std::string & reason) : StunMsgAttr(STUN_ATTR_ERROR_CODE) {
        _class = (uint8_t)(code/100);
        number = (uint8_t)(code%100);
    }

    size_t size() {
        return StunMsgAttr::size() + 4 + reason.size();
    }

    int32_t encode(uint8_t *data, size_t len) {
        length = 4 + reason.size();
        uint8_t *data_start = data;
        int32_t consumed = StunMsgAttr::encode(data, len);
        if (consumed < 0) {
            return -1;
        }
        data += consumed;
        len -= consumed;
        if (len < 4 + reason.size()) {
            return -2;
        }

        data[3] = number;
        data[2] = 0x07&_class;
        data[1] = 0;
        data[0] = 0;
        data += 4;
        if (reason.size() > 0) {
            memcpy(data, reason.data(), reason.size());
            data += reason.size();
        }
        
        return data - data_start;
    }

    uint8_t _class;
    uint8_t number;
    std::string reason;
};

struct StunUnknownAttributesAttr : public StunMsgAttr {
    StunUnknownAttributesAttr() : StunMsgAttr(STUN_ATTR_UNKNOWN_ATTRIBUTES) {

    }

    uint16_t attr[4];
};


struct StunReflectFromAttr : public StunMsgAttr {
    StunReflectFromAttr() : StunMsgAttr(STUN_ATTR_REFLECTED_FROM) {

    }

    uint8_t family; // always 0x01
    uint16_t port;
    uint32_t address;
};

};

