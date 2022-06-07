#include "client_hello.h"
using namespace mms;

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

    uint8_t session_id_len = *data;
    data++;
    len--;
    if (session_id_len > 0) {
        session_id.assign((char*)data, session_id_len);
        data += session_id_len;
        len -= session_id_len;
    }

    uint8_t cookie_len = *data;
    data++;
    len--;
    if (cookie_len > 0) {
        cookie.assign((char*)data, cookie_len);
        data += cookie_len;
        len -= cookie_len;
    }

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

int32_t ClientHello::encode(uint8_t *data, size_t len)
{
    return 0;
}