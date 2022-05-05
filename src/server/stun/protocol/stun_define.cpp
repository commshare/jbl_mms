#include <iostream>

#include "stun_define.hpp"
#include "stun_message_integrity_attr.h"
using namespace mms;

int32_t StunMsg::decode(uint8_t *data, size_t len)
{
    int32_t consumed = header.decode(data, len);
    std::cout << "header consumed:" << consumed << std::endl;
    if (consumed < 0)
    {
        return -1;
    }

    data += consumed;
    len -= consumed;
    while (len > 0)
    {
        uint16_t t = ntohs(*(uint16_t *)data);
        std::cout << "attr type:" << t << std::endl;
        switch (t)
        {
        case STUN_ATTR_MAPPED_ADDRESS:
        {
            auto mapped_addr_attr = std::unique_ptr<StunMappedAddressAttr>();
            int32_t c = mapped_addr_attr->decode(data, len);
            if (c < 0)
            {
                return -2;
            }
            data += c;
            len -= c;
            attrs.emplace_back(std::move(mapped_addr_attr));
            break;
        }
        case STUN_ATTR_RESPONSE_ADDRESS:
        {
            break;
        }
        case STUN_ATTR_CHANGE_REQUEST:
        {
            break;
        }
        case STUN_ATTR_SOURCE_ADDRESS:
        {
            break;
        }
        case STUN_ATTR_CHANGED_ADDRESS:
        {
            break;
        }
        case STUN_ATTR_USERNAME:
        {
            break;
        }
        case STUN_ATTR_PASSWORD:
        {
            break;
        }
        case STUN_ATTR_MESSAGE_INTEGRITY:
        {
            break;
        }
        case STUN_ATTR_ERROR_CODE:
        {
            break;
        }
        case STUN_ATTR_UNKNOWN_ATTRIBUTES:
        {
            break;
        }
        case STUN_ATTR_REFLECTED_FROM:
        {
            break;
        }
        }
    }
    return consumed;
}

size_t StunMsg::size(bool add_finger_print)
{
    int32_t s = 0;
    s += header.size();
    for (auto &attr : attrs)
    {
        s += attr->size();
    }

    if (add_finger_print)
    {
        s += 4 + 4; /*StunMsgAttr::size() + crc32*/
    }
    return s;
}

int32_t StunMsg::encode(uint8_t *data, size_t len, bool add_message_integrity, bool add_finger_print, const std::string &pwd)
{
    int32_t content_len = 0;
    for (auto &attr : attrs)
    {
        content_len += attr->size();
    }
    header.length = content_len;
    if (add_finger_print)
    {
        header.length += 8;
    }

    if (add_message_integrity)
    {
        header.length += 4 + 20;
    }
    uint8_t *data_start = data;
    int32_t consumed = header.encode(data, len);
    if (consumed < 0)
    {
        return -1;
    }

    data += consumed;
    len -= consumed;
    for (auto &attr : attrs)
    {
        consumed = attr->encode(data, len);
        if (consumed < 0)
        {
            return -2;
        }
        data += consumed;
        len -= consumed;
    }

    if (add_message_integrity)
    {
        auto message_integrity = std::unique_ptr<StunMessageIntegrityAttr>(new StunMessageIntegrityAttr(data_start, data - data_start, add_finger_print, pwd));
        consumed = message_integrity->encode(data, len);
        if (consumed < 0)
        {
            return -3;
        }
        data += consumed;
        len -= consumed;
    }

    if (add_finger_print)
    {
        auto finger_print = std::unique_ptr<StunFingerPrintAttr>(new StunFingerPrintAttr(data_start, data - data_start));
        consumed = finger_print->encode(data, len);
        if (consumed < 0)
        {
            return -4;
        }
        data += consumed;
        len -= consumed;
    }

    return data - data_start;
}
