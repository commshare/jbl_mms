#include <iostream>

#include "stun_define.hpp"
#include "stun_message_integrity_attr.h"
#include "stun_username_attr.h"
#include "stun_goog_network_info_attr.h"
#include "stun_ice_priority_attr.h"
#include "stun_ice_use_candidate_attr.h"
#include "stun_ice_controlled_attr.h"
#include "stun_ice_controlling_attr.h"
#include "stun_message_integrity_attr.h"
#include "stun_fingerprint_attr.h"

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
        std::cout << std::hex << "attr type:" << t << std::endl;
        switch (t)
        {
        case STUN_ATTR_MAPPED_ADDRESS:
        {
            auto mapped_addr_attr = std::unique_ptr<StunMappedAddressAttr>(new StunMappedAddressAttr);
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
            auto username_attr = std::unique_ptr<StunUsernameAttr>(new StunUsernameAttr);
            int32_t c = username_attr->decode(data, len);
            if (c < 0)
            {
                return -2;
            }
            data += c;
            len -= c;
            std::cout << "username, consumed:" << c << std::endl;
            attrs.emplace_back(std::move(username_attr));
            break;
        }
        case STUN_ATTR_PASSWORD:
        {
            break;
        }
        case STUN_ATTR_MESSAGE_INTEGRITY:
        {
            auto message_integrity_attr = std::unique_ptr<StunMessageIntegrityAttr>(new StunMessageIntegrityAttr);
            int32_t c = message_integrity_attr->decode(data, len);
            if (c < 0)
            {
                return -2;
            }
            data += c;
            len -= c;
            attrs.emplace_back(std::move(message_integrity_attr));
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
        case STUN_ATTR_GOOG_NETWORK_INFO:
        {
            auto goog_network_info_attr = std::unique_ptr<StunGoogNetworkInfoAttr>(new StunGoogNetworkInfoAttr);
            int32_t c = goog_network_info_attr->decode(data, len);
            if (c < 0)
            {
                return -2;
            }
            data += c;
            len -= c;
            attrs.emplace_back(std::move(goog_network_info_attr));
            break;
        }
        case STUN_ICE_ATTR_PRIORITY:
        {
            auto ice_priority_attr = std::unique_ptr<StunIcePriorityAttr>(new StunIcePriorityAttr);
            int32_t c = ice_priority_attr->decode(data, len);
            if (c < 0)
            {
                return -2;
            }
            data += c;
            len -= c;
            attrs.emplace_back(std::move(ice_priority_attr));
            break;
        }
        case STUN_ICE_ATTR_USE_CANDIDATE:
        {
            auto ice_use_candidate_attr = std::unique_ptr<StunIceUseCandidateAttr>(new StunIceUseCandidateAttr);
            int32_t c = ice_use_candidate_attr->decode(data, len);
            if (c < 0)
            {
                return -2;
            }
            data += c;
            len -= c;
            attrs.emplace_back(std::move(ice_use_candidate_attr));
            break;
        }
        case STUN_ICE_ATTR_ICE_CONTROLLED:
        {
            auto ice_controlled_attr = std::unique_ptr<StunIceControlledAttr>(new StunIceControlledAttr);
            int32_t c = ice_controlled_attr->decode(data, len);
            if (c < 0)
            {
                return -2;
            }
            data += c;
            len -= c;
            attrs.emplace_back(std::move(ice_controlled_attr));
            break;
        }
        case STUN_ICE_ATTR_ICE_CONTROLLING:
        {
            auto ice_controlling_attr = std::unique_ptr<StunIceControllingAttr>(new StunIceControllingAttr);
            int32_t c = ice_controlling_attr->decode(data, len);
            if (c < 0)
            {
                return -2;
            }
            data += c;
            len -= c;
            attrs.emplace_back(std::move(ice_controlling_attr));
            break;
        }
        default: {
            return -2;
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
