#include <string.h>

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
    if (msg_type == client_hello)
    {
        msg = std::unique_ptr<HandShakeMsg>(new ClientHello);
    }

    if (!msg)
    {
        return -3;
    }

    int32_t c = msg->decode(data, len);
    if (c < 0)
    {
        return -4;
    }
    data += c;
    len -= c;

    return data - data_start;
}

int32_t ClientHello::decode(uint8_t *data, size_t len)
{
    uint8_t *data_start = data;
    int32_t c = client_version.decode(data, len);
    if (c < 0)
    {
        return -1;
    }
    data += c;
    len -= c;

    c = random.decode(data, len);
    if (c < 0)
    {
        return -2;
    }
    data += c;
    len -= c;

    session_id.assign((char*)data);
    data += session_id.size() + 1;
    len -= session_id.size() + 1;

    cookie.assign((char*)data);
    data += cookie.size() + 1;
    len -= cookie.size() + 1;

    c = cipher_suites.decode(data, len);
    if (c < 0)
    {
        return -3;
    }
    data += c;
    len -= c;

    c = compression_methods.decode(data, len);
    if (c < 0)
    {
        return -4;
    }
    data += c;
    len -= c;

    if (len > 0) // has extension
    {
        c = extension.decode(data, len);
        if (c < 0) {
            return -5;
        }
        data += c;
        len -= c;
    }
    return data - data_start;
}

int32_t HelloVerifyRequest::decode(uint8_t *data, size_t len)
{
    return 0;
}