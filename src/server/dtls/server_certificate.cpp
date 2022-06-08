#include <arpa/inet.h>
#include <string.h>
#include "base/utils/utils.h"

#include "server_certificate.h"
using namespace mms;

int32_t ServerCertificate::decode(uint8_t *data, size_t len)
{//todo implement is
    return -1;
}

int32_t ServerCertificate::encode(uint8_t *data, size_t len)
{
    uint8_t *data_start = data;
    if (len < 3)
    {
        return -1;
    }
    uint8_t *p = (uint8_t*)&certificates_length;
    data[0] = p[2];
    data[1] = p[1];
    data[2] = p[0];
    data += 3;
    len -= 3;

    for (auto & cert : certificates)
    {
        int32_t consumed = cert.encode(data, len);
        if (consumed < 0)
        {
            return -2;
        }
        data += consumed;
        len -= consumed;
    }

    return data - data_start;
}

uint32_t ServerCertificate::size()
{
    uint32_t size = 3;
    certificates_length = 0;
    for (auto & cert : certificates)
    {
        uint32_t s = cert.size();
        size += s;
        certificates_length += s;
    }
    return size;
}
