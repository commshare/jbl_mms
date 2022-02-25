#include <iostream>

#include "stun_define.hpp"
using namespace mms;

int32_t StunMsg::decode(uint8_t *data, size_t len) {
    int32_t consumed = header.decode(data, len);
    std::cout << "header consumed:" << consumed << std::endl;
    if (consumed < 0) {
        return -1;
    }

    data += consumed;
    len -= consumed;
    while(len > 0) {
        uint16_t t = ntohs(*(uint16_t*)data);
        std::cout << "attr type:" << t << std::endl;
        switch(t) {
            case STUN_ATTR_MAPPED_ADDRESS    : {
                auto mapped_addr_attr = std::unique_ptr<StunMappedAddressAttr>();
                int32_t c = mapped_addr_attr->encode(data, len);
                if (c < 0) {
                    return -2;
                }
                data += c;
                len -= c;
                attrs.emplace_back(std::move(mapped_addr_attr));
                break;
            }
            case STUN_ATTR_RESPONSE_ADDRESS  : {
                break;
            }
            case STUN_ATTR_CHANGE_REQUEST    : {
                break;
            }
            case STUN_ATTR_SOURCE_ADDRESS    : {
                break;
            }
            case STUN_ATTR_CHANGED_ADDRESS   : {
                break;
            }
            case STUN_ATTR_USERNAME          : {
                break;
            }
            case STUN_ATTR_PASSWORD          : {
                break;
            }
            case STUN_ATTR_MESSAGE_INTEGRITY : {
                break;
            }
            case STUN_ATTR_ERROR_CODE        : {
                break;
            }
            case STUN_ATTR_UNKNOWN_ATTRIBUTES: {
                break;
            }
            case STUN_ATTR_REFLECTED_FROM    : {
                break;
            }
        }
    }
    return consumed;
}

