#pragma once
#include <string>
#include <optional>
#include <vector>
#include <memory>
#include <netinet/in.h>
#include <string.h>

#include "stun_message_integrity_attr.h"
#include "stun_fingerprint_attr.h"

namespace mms
{
    struct StunMessageIntegrityAttr;
    struct StunFingerPrintAttr;

    struct StunMsgHeader
    {
        uint16_t type;
        uint16_t length;
        uint8_t transaction_id[16];
        int32_t decode(uint8_t *data, size_t len)
        {
            if (len < 20)
            {
                return -1;
            }

            type = ntohs(*(uint16_t *)data);
            data += 2;
            len = ntohs(*(uint16_t *)data);
            data += 2;
            memcpy(transaction_id, data, 16);
            return 20;
        }

        int32_t encode(uint8_t *data, size_t len)
        {
            if (len < 20)
            {
                return -1;
            }
            *(uint16_t *)data = htons(type);
            data += 2;
            *(uint16_t *)data = htons(length);
            data += 2;
            memcpy(data, transaction_id, 16);
            return 20;
        }

        size_t size()
        {
            return 20;
        }
    };

    struct StunMsg
    {
        StunMsgHeader header;
        std::vector<std::unique_ptr<StunMsgAttr>> attrs;
        std::unique_ptr<StunMessageIntegrityAttr> msg_integrity_attr;
        std::unique_ptr<StunFingerPrintAttr> fingerprint_attr;

        void addAttr(std::unique_ptr<StunMsgAttr> attr)
        {
            attrs.emplace_back(std::move(attr));
        }

        uint16_t type()
        {
            return header.type;
        }

        int32_t decode(uint8_t *data, size_t len);

        size_t size(bool add_finger_print = false);

        virtual int32_t encode(uint8_t *data, size_t len, bool add_message_integrity = false, bool add_finger_print = false, const std::string &pwd = "");
    };
};