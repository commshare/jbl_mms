#include <iostream>
#include "stun_username_attr.h"
#include "base/utils/utils.h"
using namespace mms;

size_t StunUsernameAttr::size()
{
    if (remote_user_name_.size() > 0) 
    {
        return StunMsgAttr::size() + local_user_name_.size() + 1 + remote_user_name_.size();
    }
    else 
    {
        return StunMsgAttr::size() + local_user_name_.size();
    }
}

int32_t StunUsernameAttr::encode(uint8_t *data, size_t len)
{
    length = remote_user_name_.size()>0?local_user_name_.size() + 1 + remote_user_name_.size():local_user_name_.size();
    uint8_t *data_start = data;
    int32_t consumed = StunMsgAttr::encode(data, len);
    if (consumed < 0)
    {
        return -1;
    }
    data += consumed;
    len -= consumed;
    if (len < length)
    {
        return -2;
    }

    memcpy(data, local_user_name_.data(), local_user_name_.size());
    uint8_t * tmp = data;
    tmp += local_user_name_.size();
    if (remote_user_name_.size() > 0) {
        memcpy(tmp, ":", 1);
        tmp++;
        memcpy(tmp, remote_user_name_.data(), remote_user_name_.size());
        tmp += remote_user_name_.size();
    }

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

    std::string_view user_name((char*)data, length);
    std::vector<std::string_view> vs;
    vs = Utils::split(user_name, ":");
    if (vs.size() == 1) {
        local_user_name_ = vs[0];
    } else if (vs.size() == 2) {
        local_user_name_ = vs[0];
        remote_user_name_ = vs[1];
    }

    data += ((length+3)>>2)<<2;
    return data - data_start;
}