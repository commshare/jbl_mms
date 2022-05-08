#include <iostream>
#include "stun_username_attr.h"
using namespace mms;

size_t StunUsernameAttr::size()
{
    return StunMsgAttr::size() + user_name_.size();
}

int32_t StunUsernameAttr::encode(uint8_t *data, size_t len)
{
    length = user_name_.size();
    uint8_t *data_start = data;
    int32_t consumed = StunMsgAttr::encode(data, len);
    if (consumed < 0)
    {
        return -1;
    }
    data += consumed;
    len -= consumed;
    if (len < user_name_.size())
    {
        return -2;
    }

    memcpy(data, user_name_.data(), user_name_.size());
    data += ((length+3)>>2)<<2;
    return data - data_start;
}

int32_t StunUsernameAttr::decode(uint8_t *data, size_t len)
{
    uint8_t *data_start = data;
    int32_t consumed = StunMsgAttr::decode(data, len);
    if (consumed < 0)
    {
        return -1;
    }
    data += consumed;
    len -= consumed;

    if (len <= 0)
    {
        return -2;
    }
    user_name_.assign((char*)data, length);
    data += ((length+3)>>2)<<2;
    return data - data_start;
}