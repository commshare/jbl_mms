#include <iostream>

#include <string.h>
#include <arpa/inet.h>
#include <stdint.h>

#include "signature_algorithm.h"
using namespace mms;

int32_t SignatureAndHashAlgorithmExt::decode(uint8_t *data, size_t len)
{
    uint8_t *data_start = data;
    int32_t c = header.decode(data, len);
    if (c < 0)
    {
        return -1;
    }
    data += c;
    len -= c;

    if (len < 2)
    {
        return -2;
    }

    uint16_t signature_hash_algorithms_length = ntohs(*(uint16_t *)data);
    uint16_t count = signature_hash_algorithms_length > 1;
    data += 2;
    len -= 2;
    while (count > 0)
    {
        if (len < 2)
        {
            return -3;
        }

        SignatureAndHashAlgorithm sha;
        c = sha.decode(data, len);
        if (c < 0)
        {
            return -4;
        }
        data += c;
        count--;
        len -= c;
        shas.push_back(sha);
    }

    return data - data_start;
}

uint32_t SignatureAndHashAlgorithmExt::size()
{
    uint32_t size = 0;
    size += header.size();
    size += 2; // shas length
    size += shas.size() * 2;
    return size;
}

int32_t SignatureAndHashAlgorithmExt::encode(uint8_t *data, size_t len)
{
    uint8_t *data_start = data;
    int32_t c = header.encode(data, len);
    if (c < 0)
    {
        return -1;
    }
    data += c;
    len -= c;

    uint16_t *plen = (uint16_t *)data;
    data += 2;
    len -= 2;
    if (len < 0)
    {
        return -2;
    }

    uint32_t content_len = 0;
    for (auto &sha : shas)
    {
        c = sha.encode(data, len);
        data += c;
        len -= c;
        content_len += c;
    }

    *plen = htons(content_len);
    return data - data_start;
}
