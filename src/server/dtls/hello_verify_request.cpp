#include "hello_verify_request.h"
using namespace mms;

int32_t HelloVerifyRequest::decode(uint8_t *data, size_t len)
{
    return 0;
}

uint32_t HelloVerifyRequest::size()
{
    uint32_t size = 0;
    size += server_version.size();
    size += 1;
    size += cookie.size();
    return size;
}

int32_t HelloVerifyRequest::encode(uint8_t *data, size_t len)
{
    uint8_t *data_start = data;
    int32_t consumed = server_version.encode(data, len);
    if (consumed < 0)
    {
        return -1;
    }
    data += consumed;
    len -= consumed;

    *data = cookie.size();
    data++;
    len--;
    
    if (len < cookie.size())
    {
        return -2;
    }
    memcpy(data, cookie.data(), cookie.size());
    data += cookie.size();
    len -= cookie.size();

    return data - data_start;
}