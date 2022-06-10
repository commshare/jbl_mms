#include <arpa/inet.h>
#include <string.h>
#include <iostream>

#include "base/utils/utils.h"

#include "dtls_define.h"
#include "dtls_handshake.h"

#include "./extension/dtls_use_srtp.h"
#include "./extension/signature_algorithm.h"
using namespace mms;

int32_t DtlsProtocolVersion::decode(uint8_t *data, size_t len)
{
    if (len < 2)
    {
        return -1;
    }

    major = data[0];
    minor = data[1];
    return 2;
}

int32_t DtlsProtocolVersion::encode(uint8_t *data, size_t len)
{
    if (len < 2)
    {
        return -1;
    }
    data[0] = major;
    data[1] = minor;
    return 2;
}

int32_t Random::decode(uint8_t *data, size_t len)
{
    uint8_t *data_start = data;
    if (len < 4)
    {
        return -1;
    }

    gmt_unix_time = ntohl(*(uint32_t *)data);
    data += 4;
    len -= 4;

    if (len < 28)
    {
        return -2;
    }
    memcpy(random_bytes, data, 28);
    data += 28;
    memcpy(random_raw, data_start, 32);
    return data - data_start;
}

int32_t Random::encode(uint8_t *data, size_t len)
{
    uint8_t *data_start = data;
    if (len < 4)
    {
        return -1;
    }

    *(uint32_t *)data = htonl(gmt_unix_time);
    data += 4;
    len -= 4;

    if (len < 28)
    {
        return -2;
    }
    memcpy(data, random_bytes, 28);
    data += 28;
    memcpy(random_raw, data_start, 32);
    return data - data_start;
}

void Random::genRandom()
{
    std::string s = Utils::randStr(28);
    memcpy(random_bytes, s.data(), 28);
    gmt_unix_time = time(NULL);
}

int32_t DtlsHeader::decode(uint8_t *data, size_t len)
{
    uint8_t *data_start = data;
    if (len < 1)
    {
        return -1;
    }

    type = (ContentType)data[0];
    data++;
    len--;

    int32_t c = version.decode(data, len);
    if (c < 0)
    {
        return -2;
    }
    data += c;
    len -= c;

    if (len < 2)
    {
        return -3;
    }
    epoch = ntohs(*(uint16_t *)data);
    data += 2;
    len -= 2;

    if (len < 6)
    {
        return -4;
    }
    sequence_number = 0;
    uint8_t *p = (uint8_t *)&sequence_number;
    p[0] = data[5];
    p[1] = data[4];
    p[2] = data[3];
    p[3] = data[2];
    p[4] = data[1];
    p[5] = data[0];
    data += 6;
    len -= 6;

    if (len < 2)
    {
        return -3;
    }
    length = ntohs(*(uint16_t *)data);
    data += 2;
    len -= 2;
    return data - data_start;
}

int32_t DtlsHeader::encode(uint8_t *data, size_t len)
{
    uint8_t *data_start = data;
    if (len < 1)
    {
        return -1;
    }
    data[0] = type;
    data++;
    len--;

    int32_t c = version.encode(data, len);
    if (c < 0)
    {
        return -2;
    }
    data += c;
    len -= c;

    if (len < 2)
    {
        return -3;
    }
    *(uint16_t *)data = htons(epoch);
    data += 2;
    len -= 2;

    if (len < 6)
    {
        return -4;
    }
    uint8_t *p = (uint8_t *)&sequence_number;
    data[0] = p[5];
    data[1] = p[4];
    data[2] = p[3];
    data[3] = p[2];
    data[4] = p[1];
    data[5] = p[0];
    data += 6;
    len -= 6;

    if (len < 2)
    {
        return -5;
    }
    *(uint16_t *)data = htons(length);
    data += 2;
    return data - data_start;
}

int32_t DTLSCiphertext::decode(uint8_t *data, size_t len)
{
    uint8_t *data_start = data;
    int32_t c = header.decode(data, len);
    if (c < 0)
    {
        return -1;
    }
    data += c;
    len -= c;

    if (header.type == handshake)
    {
        msg = std::unique_ptr<DtlsMsg>(new HandShake);
    }

    if (!msg)
    {
        return -2;
    }

    c = msg->decode(data, len);
    if (c < 0)
    {
        return -3;
    }
    data += c;
    len -= c;

    return data - data_start;
}

int32_t DTLSCiphertext::encode(uint8_t *data, size_t len)
{
    if (!msg)
    {
        return -1;
    }

    uint8_t *data_start = data;
    header.length = msg->size();
    int32_t c = header.encode(data, len);
    if (c < 0)
    {
        return -1;
    }

    data += c;
    len -= c;

    c = msg->encode(data, len);
    if (c < 0)
    {
        return -2;
    }

    data += c;
    len -= c;
    return data - data_start;
}

int32_t CipherSuites::decode(uint8_t *data, size_t len)
{
    uint8_t *data_start = data;
    if (len < 2)
    {
        return -1;
    }

    uint16_t length = ntohs(*(uint16_t *)data);
    data += 2;
    len -= 2;

    int16_t count = length >> 1;
    if (len < length)
    {
        return -2;
    }

    while (count > 0)
    {
        uint16_t cipher = ntohs(*(uint16_t *)data);
        cipher_suites.push_back(cipher);
        data += 2;
        len -= 2;
        count--;
    }

    return data - data_start;
}

uint32_t CipherSuites::size()
{
    uint32_t s = 2;
    s += cipher_suites.size() * 2;
    return s;
}

int32_t CompressionMethods::decode(uint8_t *data, size_t len)
{
    uint8_t *data_start = data;
    if (len < 1)
    {
        return -1;
    }

    uint8_t length = data[0];
    data += 1;
    len -= 1;
    if (len < length)
    {
        return -2;
    }

    while (length > 0)
    {
        if (len < 1)
        {
            return -3;
        }
        compression_methods.push_back(data[0]);
        data += 1;
        len -= 1;
        length--;
    }

    return data - data_start;
}

uint32_t CompressionMethods::size()
{
    uint32_t s = 1;
    s += compression_methods.size();
    return s;
}

int32_t DtlsExtension::decode(uint8_t *data, size_t len)
{
    uint8_t *data_start = data;
    uint16_t length = ntohs(*(uint16_t *)data);
    data += 2;
    len -= 2;
    while (length > 0)
    {
        int32_t c;
        ExtensionType t = (ExtensionType)ntohs(*(uint16_t *)data);
        if (t == use_srtp)
        {
            std::unique_ptr<DtlsExtItem> item = std::unique_ptr<DtlsExtItem>(new UseSRtpExt);
            c = item->decode(data, len);
            if (c < 0)
            {
                return -1;
            }
            data += c;
            length -= c;
            len -= c;
        }
        else if (t == signature_algorithms)
        {
            std::unique_ptr<SignatureAndHashAlgorithmExt> item = std::unique_ptr<SignatureAndHashAlgorithmExt>(new SignatureAndHashAlgorithmExt);
            c = item->decode(data, length);
            if (c < 0)
            {
                return -2;
            }
            data += c;
            length -= c;
            len -= c;
        }
        else
        {
            std::unique_ptr<DtlsExtItem> item = std::unique_ptr<DtlsExtItem>(new UnknownExtItem);
            c = item->decode(data, len);
            if (c < 0)
            {
                return -3;
            }
            data += c;
            length -= c;
            len -= c;
        }
    }
    return data - data_start;
}

int32_t DtlsExtension::encode(uint8_t *data, size_t len)
{
    if (extensions.size() <= 0) 
    {
        return 0;
    }

    uint8_t *data_start = data;
    if (len < 2)
    {
        return -1;
    }
    *(uint16_t *)data = htons(size() - 2);
    data += 2;
    len -= 2;

    for (auto &p : extensions)
    {
        int32_t c = p.second->encode(data, len);
        if (c < 0)
        {
            return -2;
        }
        data += c;
        len -= c;
    }

    return data - data_start;
}

uint32_t DtlsExtension::size()
{
    if (extensions.size() <= 0) 
    {
        return 0;
    }
    uint32_t size = 0;
    size += 2; // length
    for (auto &p : extensions)
    {
        size += p.second->size();
    }
    return size;
}

int32_t UnknownExtItem::decode(uint8_t *d, size_t len)
{
    uint8_t *data_start = d;
    int32_t c = header.decode(d, len);
    if (c < 0)
    {
        return -1;
    }
    d += c;
    len -= c;
    d += header.length;
    len -= header.length;
    return d - data_start;
}

int32_t UnknownExtItem::encode(uint8_t *d, size_t len)
{
    return 0;
}

int32_t DtlsExtensionHeader::decode(uint8_t *data, size_t len)
{
    uint8_t *data_start = data;
    if (len < 2)
    {
        return -1;
    }
    type = (ExtensionType)ntohs(*(uint16_t *)data);
    data += 2;
    len -= 2;

    if (len < 2)
    {
        return -2;
    }
    length = ntohs(*(uint16_t *)data);
    data += 2;
    len -= 2;
    return data - data_start;
}

int32_t DtlsExtensionHeader::encode(uint8_t *data, size_t len)
{
    uint8_t *data_start = data;
    if (len < 4)
    {
        return -1;
    }

    *(uint16_t *)data = htons(type);
    data += 2;
    *(uint16_t *)data = htons(length);
    data += 2;
    len -= 4;
    return data - data_start;
}