#pragma once
#include <string>
#include <vector>
#include <stdint.h>

#include "dtls_define.h"

namespace mms
{
    enum HandshakeType
    {
        hello_request = 0,
        client_hello = 1,
        server_hello = 2,
        hello_verify_request = 3, // New field
        certificate = 11,
        server_key_exchange = 12,
        certificate_request = 13,
        server_hello_done = 14,
        certificate_verify = 15,
        client_key_exchange = 16,
        finished = 20
    };

    struct HandShakeMsg
    {
        HandShakeMsg() = default;
        virtual int32_t decode(uint8_t *data, size_t len) = 0;
        virtual int32_t encode(uint8_t *data, size_t len) = 0;
        virtual uint32_t size() = 0;
    };

    struct HandShake : public DtlsMsg
    {
        HandshakeType msg_type;       /* handshake type */
        uint32_t length;              /* bytes in message (24bit) */
        uint16_t message_seq = 0;     // New field
        uint32_t fragment_offset = 0; // New field(24bit)
        uint32_t fragment_length = 0; // New field(24bit)

        std::unique_ptr<HandShakeMsg> msg;
        void setMsg(std::unique_ptr<HandShakeMsg> v)
        {
            msg = std::move(v);
        }

        int32_t decode(uint8_t *data, size_t len);
        int32_t encode(uint8_t *data, size_t len);
        uint32_t size();
        HandshakeType getType() const
        {
            return msg_type;
        }

        void setType(HandshakeType type)
        {
            msg_type = type;
        }

        void setMessageSeq(uint16_t val)
        {
            message_seq = val;
        }
    };
};