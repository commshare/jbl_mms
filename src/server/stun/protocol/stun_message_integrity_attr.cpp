#include "stun_message_integrity_attr.h"
using namespace mms;

StunMessageIntegrityAttr::StunMessageIntegrityAttr(uint8_t *data, size_t len, bool has_finger_print, const std::string & pwd) : StunMsgAttr(STUN_ATTR_MESSAGE_INTEGRITY)
{
    unsigned int digest_len;
    hmac_sha1.resize(20);
    size_t len_with_message_integrity = len;
    if (has_finger_print) {
        len_with_message_integrity -= 24;
    }
    *(uint16_t *)(data+2) = htons(len_with_message_integrity);
    HMAC_CTX *ctx = HMAC_CTX_new();
    HMAC_Init_ex(ctx, pwd.c_str(), pwd.size(), EVP_sha1(), NULL);
    HMAC_Update(ctx, (const unsigned char *)data, len_with_message_integrity);
    HMAC_Final(ctx, (unsigned char *)hmac_sha1.data(), &digest_len);
    HMAC_CTX_free(ctx);
    *(uint16_t *)(data+2) = htons(len);
}

size_t StunMessageIntegrityAttr::size()
{
    return StunMsgAttr::size() + 20;
}

int32_t StunMessageIntegrityAttr::encode(uint8_t *data, size_t len)
{
    length = 20;
    uint8_t *data_start = data;
    int32_t consumed = StunMsgAttr::encode(data, len);
    if (consumed < 0)
    {
        return -1;
    }
    data += consumed;
    len -= consumed;
    if (len < 20)
    {
        return -2;
    }
    memcpy(data, hmac_sha1.data(), 20);
    data += 20;
    return data - data_start;
}

int32_t StunMessageIntegrityAttr::decode(uint8_t *data, size_t len)
{
    uint8_t *data_start = data;
    int32_t consumed = StunMsgAttr::decode(data, len);
    if (consumed < 0)
    {
        return -1;
    }
    data += consumed;
    len -= consumed;

    if (length != 20)
    {
        return -2;
    }

    if (len < 20) {
        return -3;
    }

    hmac_sha1.assign((char*)data, length);
    data += length;
    // len -= consumed;
    return data - data_start;
}