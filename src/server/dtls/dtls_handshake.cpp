#include <string.h>
#include <iostream>
#include <arpa/inet.h>

#include "client_hello.h"
#include "server_hello.h"
#include "client_key_exchange.h"

#include "dtls_handshake.h"
using namespace mms;

int32_t HandShake::decode(uint8_t *data, size_t len)
{
    uint8_t *data_start = data;
    if (len < 1)
    {
        return -1;
    }
    msg_type = (HandshakeType)data[0];
    data++;
    len--;

    if (len < 3)
    {
        return -2;
    }
    uint8_t *p = (uint8_t *)&length;
    p[0] = data[2];
    p[1] = data[1];
    p[2] = data[0];
    data += 3;
    len -= 3;

    if (len < 2)
    {
        return -3;
    }
    message_seq = ntohs(*(uint16_t *)data);
    data += 2;
    len -= 2;

    if (len < 3)
    {
        return -4;
    }
    p = (uint8_t *)&fragment_offset;
    p[0] = data[2];
    p[1] = data[1];
    p[2] = data[0];
    data += 3;
    len -= 3;

    if (len < 3)
    {
        return -5;
    }
    p = (uint8_t *)&fragment_length;
    p[0] = data[2];
    p[1] = data[1];
    p[2] = data[0];
    data += 3;
    len -= 3;

    if (msg_type == client_hello)
    {
        msg = std::unique_ptr<HandShakeMsg>(new ClientHello);
    }
    else if (msg_type == server_hello)
    {
        msg = std::unique_ptr<HandShakeMsg>(new ServerHello);
    }
    else if (msg_type == client_key_exchange)
    {
        msg = std::unique_ptr<HandShakeMsg>(new ClientKeyExchange);
    }

    if (!msg)
    {
        return -6;
    }

    int32_t c = msg->decode(data, len);
    if (c < 0)
    {
        std::cout << "server decode failed, code:" << c << std::endl;
        return -7;
    }
    data += c;
    len -= c;

    return data - data_start;
}

int32_t HandShake::encode(uint8_t *data, size_t len)
{
    uint8_t *data_start = data;
    if (len < 1)
    {
        return -1;
    }
    data[0] = msg_type;
    data++;
    len--;

    if (len < 3)
    {
        return -2;
    }
    uint8_t *plen = data;
    data += 3;
    len -= 3;

    if (len < 2)
    {
        return -3;
    }
    *((uint16_t *)data) = htons(message_seq);
    data += 2;
    len -= 2;

    if (len < 3)
    {
        return -4;
    }
    uint8_t *pfragment_offset = data;
    data += 3;
    len -= 3;

    if (len < 3)
    {
        return -5;
    }
    uint8_t *pfragment_length = data;
    data += 3;
    len -= 3;

    uint32_t content_len = 0;
    if (msg)
    {
        int32_t c = msg->encode(data, len);
        if (c < 0)
        {
            return -2;
        }
        data += c;
        len -= c;
        content_len += c;
    }
    uint8_t *p = (uint8_t *)&content_len;
    plen[0] = p[2];
    plen[1] = p[1];
    plen[2] = p[0];

    fragment_length = content_len;
    p = (uint8_t *)&fragment_length;
    pfragment_length[0] = p[2];
    pfragment_length[1] = p[1];
    pfragment_length[2] = p[0];

    fragment_offset = 0;
    p = (uint8_t *)&fragment_offset;
    pfragment_offset[0] = p[2];
    pfragment_offset[1] = p[1];
    pfragment_offset[2] = p[0];

    return data - data_start;
}

uint32_t HandShake::size()
{
    uint32_t s = 12;
    s += msg->size();
    return s;
}