#include <arpa/inet.h>
#include <string.h>
#include "base/utils/utils.h"

#include "server_hello.h"
using namespace mms;

int32_t ServerHello::decode(uint8_t *data, size_t len)
{
    uint8_t *data_start = data;
    int32_t c = server_version.decode(data, len);
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

    session_id.assign((char *)data);
    data += session_id.size() + 1;
    len -= session_id.size() + 1;

    if (len < 2)
    {
        return -3;
    }

    cipher_suite = ntohs(*(uint16_t *)data);
    data += 2;
    len -= 2;

    compression_method = data[0];
    data += 1;
    len -= 1;

    if (len > 0) // has extension
    {
        c = extension.decode(data, len);
        if (c < 0)
        {
            return -5;
        }
        data += c;
        len -= c;
    }
    return data - data_start;
}

void ServerHello::genRandom()
{
    random.genRandom();
}

int32_t ServerHello::encode(uint8_t *data, size_t len)
{
    uint8_t *data_start = data;
    int32_t consumed = server_version.encode(data, len);
    if (consumed < 0)
    {
        return -1;
    }
    data += consumed;
    len -= consumed;

    consumed = random.encode(data, len);
    if (consumed < 0)
    {
        return -2;
    }
    data += consumed;
    len -= consumed;

    uint8_t session_id_len = session_id.size();
    *data = session_id_len;
    data++;
    len--;

    if (len < session_id_len)
    {
        return -3;
    }
    memcpy(data, session_id.data(), session_id_len);
    data += session_id_len;
    len -= session_id_len;

    if (len < 2)
    {
        return -4;
    }
    *(uint16_t *)data = htons(cipher_suite);
    data += 2;
    len -= 2;

    if (len < 1)
    {
        return -5;
    }
    *data = compression_method;
    data++;
    len--;

    consumed = extension.encode(data, len);
    if (consumed < 0)
    {
        return -6;
    }
    data += consumed;
    len -= consumed;

    return data - data_start;
}

uint32_t ServerHello::size()
{
    uint32_t size = 0;
    size += server_version.size();
    size += random.size();
    size += session_id.size() + 1;
    size += sizeof(cipher_suite);
    size += sizeof(compression_method);
    size += extension.size();
}
