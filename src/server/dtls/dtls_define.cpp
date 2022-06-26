#include <arpa/inet.h>
#include <string.h>
#include <iostream>

#include "openssl/aes.h"
#include "base/utils/utils.h"

#include "dtls_define.h"
#include "dtls_handshake.h"
#include "change_cipher_spec.h"

#include "./extension/dtls_use_srtp.h"
#include "./extension/signature_algorithm.h"

#include "tls_prf.h"

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
    memcpy(random_raw, data_start, 32);
    data += 28;
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
    memcpy(random_raw, data_start, 32);
    data += 28;
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

int32_t DTLSPlaintext::decode(uint8_t *data, size_t len)
{
    uint8_t *data_start = data;
    int32_t c = header.decode(data, len);
    if (c < 0)
    {
        return -1;
    }
    data += c;
    len -= c;

    if (header.epoch == 0)
    {
        if (header.type == handshake)
        {
            msg = std::unique_ptr<DtlsMsg>(new HandShake);
        }
        else if (header.type == change_cipher_spec)
        {
            msg = std::unique_ptr<DtlsMsg>(new ChangeCipherSpec);
        }
    }
    else
    {
        msg = std::unique_ptr<DtlsMsg>(new GenericBlockCipher);
        std::cout << "decode ciper text" << std::endl;
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
    raw_data.assign((char*)data_start, data - data_start);
    return data - data_start;
}

int32_t DTLSPlaintext::encode(uint8_t *data, size_t len)
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

RSA_AES128_SHA1_Cipher::RSA_AES128_SHA1_Cipher() 
{
    ciper_id = TLS_RSA_WITH_AES_128_CBC_SHA;
    ciper_type = block;
    mac_length = 20;
    mac_key_length = 20;
    record_iv_length = 16;
    enc_key_length = 16;
    key_exchange_algorithm = CipherSuiteKeyExchangeAlgorithm_RSA;
    initialized = false;
}

// @https://datatracker.ietf.org/doc/html/rfc5246#page-95
// 生成key block及key material
//   To generate the key material, compute
//   key_block = PRF(SecurityParameters.master_secret,
//                   "key expansion",
//                   SecurityParameters.server_random +
//                   SecurityParameters.client_random);
//                       Key      IV   Block
// Cipher        Type    Material  Size  Size
// ------------  ------  --------  ----  -----
// NULL          Stream      0       0    N/A
// RC4_128       Stream     16       0    N/A
// 3DES_EDE_CBC  Block      24       8      8
// AES_128_CBC   Block      16      16     16
// AES_256_CBC   Block      32      16     16

// MAC       Algorithm    mac_length  mac_key_length
// --------  -----------  ----------  --------------
// NULL      N/A              0             0
// MD5       HMAC-MD5        16            16
// SHA       HMAC-SHA1       20            20
// SHA256    HMAC-SHA256     32            32
void RSA_AES128_SHA1_Cipher::init(const std::string & master_secret, const std::string & client_random, const std::string & server_random, bool client)
{
    is_client = client;
    std::string key_material_seed;
    key_material_seed.append((char *)server_random.data(), 32);
    key_material_seed.append((char *)client_random.data(), 32);
    int32_t key_block_size = 2 * (mac_key_length + enc_key_length + record_iv_length); // AES_128_CBC AND SHA
    std::string key_block = PRF(master_secret, "key expansion", key_material_seed, key_block_size);
    int32_t off = 0;
    client_write_MAC_key.assign(key_block.data() + off, mac_key_length);
    off += mac_key_length;
    server_write_MAC_key.assign(key_block.data() + off, mac_key_length);
    off += mac_key_length;
    client_write_key.assign(key_block.data() + off, enc_key_length);
    off += enc_key_length;
    server_write_key.assign(key_block.data() + off, enc_key_length);
    off += enc_key_length;
    client_write_IV.assign(key_block.data() + off, record_iv_length);
    off += record_iv_length;
    server_write_IV.assign(key_block.data() + off, record_iv_length);
    off += record_iv_length;
}

int32_t RSA_AES128_SHA1_Cipher::decrypt(const std::string & iv, const std::string & in, std::string & out)
{
    int ret;
    AES_KEY key;
    if (is_client)
    {
        ret = AES_set_decrypt_key((unsigned char *)server_write_key.data(), 128, &key);
        if (0 != ret) 
        {
            return -1;
        }
    }
    else
    {
        ret = AES_set_decrypt_key((unsigned char *)client_write_key.data(), 128, &key);
        if (0 != ret) 
        {
            return -1;
        }
    }

    out.resize(in.size());
    AES_cbc_encrypt((unsigned char*)in.data(), (unsigned char*)out.data(), in.size(), &key, (unsigned char *)iv.data(), AES_DECRYPT);
    
    return 0;
}

int32_t RSA_AES128_SHA1_Cipher::encrypt(const std::string & iv, const std::string & in, std::string & out)
{
    int ret;
    AES_KEY key;
    if (is_client)
    {
        ret = AES_set_decrypt_key((unsigned char *)client_write_key.data(), 128, &key);
        if (0 != ret) 
        {
            return -1;
        }
    }
    else
    {
        ret = AES_set_decrypt_key((unsigned char *)server_write_key.data(), 128, &key);
        if (0 != ret) 
        {
            return -1;
        }
    }

    out.resize(in.size());
    AES_cbc_encrypt((unsigned char*)in.data(), (unsigned char*)out.data(), in.size(), &key, (unsigned char *)iv.data(), AES_ENCRYPT);
    return 0;
}