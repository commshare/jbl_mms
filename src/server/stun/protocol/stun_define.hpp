#pragma once
#include <vector>
#include <memory>
#include <string>
#include <netinet/in.h>
#include <string.h>

#include "base/utils/utils.h"

/*
Family：IP类型，0x01-IPV4、0x02-IPV6
Port：端口
Address：IP地址

XOR-MAPPED-ADDRESS：与MAPPED-ADDRESS属性基本相同，区别在于反射地址经过一次异或（XOR）处理，异或运算是其自身的逆运算，客户端经过一次异或运算获得真实的反射地址。解决ALG篡改地址和端口的问题。

USERNAME：用户名，用于消息完整性，在webrtc中的规则为 “对端的ice-ufrag：自己的ice-ufrag”，其中ice-ufrag已通过提议/应答的SDP信息进行交互。

MESSAGE-INTEGRITY：STUN 消息的 HMAC-SHA1 值，长度 20 字节，用于消息完整性认证。详细的计算方式后续进行举例说明。

FINGERPRINT：指纹认证，此属性可以出现在所有的 STUN 消息中，该属性用于区分 STUN 数据包与其他协议的包。属性的值为采用 CRC32 方式计算 STUN 消息直到但不包括FINGERPRINT 属性的的结果，并与 32 位的值 0x5354554e 异或。

ERROR-CODE：属性用于error response报文中。其中包含了300-699表示的错误码，以及一个UTF-8格式的文字出错信息（Reason phrase）。

REALM：此属性可能出现在请求和响应中。在请求中表示长期资格将在认证中使用。当在错误响应中出现表示服务器希望客户使用长期资格来进行认证。

NONCE：出现在请求和响应消息中的一段字符串。

UNKNOWN-ATTRIBUTES：此属性只在错误代码为420的的错误响应中出现。

SOFTWARE：此属性用于代理发送消息时包含版本的描述。它用于客户端和服务器。它的值包括制造商和版本号。该属性对于协议的运行没有任何影响，仅为诊断和调试目的提供服务。SOFTWARE属性是个可变长度的，采用UTF-8编码的小于128个字符的序列号

ALTERNATE-SERVER：属性表示 STUN 客户可以尝试的不同的 STUN 服务器地址。属性格式与 MAPPED-ADDRESS 相同。

在Binding请求中通常需要包含一些特殊的属性,以在ICE进行连接性检查的时候提供必要信息，详细的属性如下所示：

PRIORITY 和 USE-CANDIDATE：终端必须在其request中包含PRIORITY属性,指明其优先级,优先级由公式计算而得。如果有需要也可以给出特别指定的候选(即USE-CANDIDATE属性)。

ICE-CONTROLLED和ICE-CONTROLLING： ICE流程中定义了两种角色:controlling和controlled。不同的角色在candidate pair优先级的计算，pair nominate的决策上有所不同。一般流程下，会话的双发各自的角色选择是与会话协商的流程相关的。offerer是controlling，answerer是controlled。

ICE-CONTROLLED或者是ICE-CONTROLLING，这两个属性都会携带一个Tie breaker这样的字段，其中包含 一个本机产生的随机值。收到该bind request的一方会检查这两个字段，如果和当前本机的role冲突，则检查本机的tie breaker值和消息中携带的tie breaker值进行判定本机合适的role。判定的方法为Tie breaker值大的一方为controlling。如果判定本端变更角色，这直接修改；如果判定对端变更角色，则对此bind request发送487错误响应，收到此错误响应的一方变更角色即可。
————————————————
版权声明：本文为CSDN博主「zyl_『码农修仙』」的原创文章，遵循CC 4.0 BY-SA版权协议，转载请附上原文出处链接及本声明。
原文链接：https://blog.csdn.net/u012538729/article/details/115694308
*/

#define STUN_BINDING_REQUEST                0x0001
#define STUN_BINDING_RESPONSE               0x0101
#define STUN_BINDING_ERROR_RESPONSE         0x0111
#define STUN_SHARED_SECRET_REQUEST          0x0002
#define STUN_SHARED_SECRET_RESPONSE         0x0102
#define STUN_SHARED_SECRET_ERROR_RESPONSE   0x0112

#define STUN_ATTR_MAPPED_ADDRESS            0x0001
#define STUN_ATTR_RESPONSE_ADDRESS          0x0002
#define STUN_ATTR_CHANGE_REQUEST            0x0003
#define STUN_ATTR_SOURCE_ADDRESS            0x0004
#define STUN_ATTR_CHANGED_ADDRESS           0x0005
#define STUN_ATTR_USERNAME                  0x0006
#define STUN_ATTR_PASSWORD                  0x0007
#define STUN_ATTR_MESSAGE_INTEGRITY         0x0008
#define STUN_ATTR_ERROR_CODE                0x0009
#define STUN_ATTR_UNKNOWN_ATTRIBUTES        0x000a
#define STUN_ATTR_REFLECTED_FROM            0x000b

#define STUN_ATTR_SOFTWARE                  0x8022
#define STUN_ATTR_ALTERNATE_SERVER          0x8023
#define STUN_ATTR_FINGERPRINT               0x8028

#define STUN_ATTR_GOOG_NETWORK_INFO         0xc057

/*
21.2.  STUN Attributes

   This section registers four new STUN attributes per the procedures in
   [RFC5389].

      0x0024 PRIORITY
      0x0025 USE-CANDIDATE
      0x8029 ICE-CONTROLLED
      0x802A ICE-CONTROLLING
*/
#define STUN_ICE_ATTR_PRIORITY              0x0024
#define STUN_ICE_ATTR_USE_CANDIDATE         0x0025
#define STUN_ICE_ATTR_ICE_CONTROLLED        0x8029
#define STUN_ICE_ATTR_ICE_CONTROLLING       0x802A

namespace mms
{

    struct StunMsg;
    struct StunMsgHeader
    {
        uint16_t type;
        uint16_t length;
        uint8_t transaction_id[16];
        int32_t decode(uint8_t *data, size_t len)
        {
            if (len < 20)
            {
                return -1;
            }

            type = ntohs(*(uint16_t *)data);
            data += 2;
            len = ntohs(*(uint16_t *)data);
            data += 2;
            memcpy(transaction_id, data, 16);
            return 20;
        }

        int32_t encode(uint8_t *data, size_t len)
        {
            if (len < 20)
            {
                return -1;
            }
            *(uint16_t *)data = htons(type);
            data += 2;
            *(uint16_t *)data = htons(length);
            data += 2;
            memcpy(data, transaction_id, 16);
            return 20;
        }

        size_t size()
        {
            return 20;
        }
    };

    struct StunMsgAttr
    {
        StunMsgAttr()
        {
        }

        StunMsgAttr(uint16_t t) : type(t)
        {
        }

        virtual size_t size()
        {
            return 4;
        }

        virtual int32_t encode(uint8_t *data, size_t len)
        {
            if (len < 4)
            {
                return -1;
            }
            *(uint16_t *)data = htons(type);
            data += 2;
            *(uint16_t *)data = htons(length);
            return 4;
        }

        virtual int32_t decode(uint8_t *data, size_t len)
        {
            if (len < 4)
            {
                return -1;
            }
            type = ntohs(*(uint16_t *)data);
            data += 2;
            length = ntohs(*(uint16_t *)data);
            data += 2;
            return 4;
        }

        uint16_t getType()
        {
            return type;
        }

        uint16_t type;
        uint16_t length;
    };

    struct StunMsg
    {
        StunMsgHeader header;
        std::vector<std::unique_ptr<StunMsgAttr>> attrs;

        void addAttr(std::unique_ptr<StunMsgAttr> attr)
        {
            attrs.emplace_back(std::move(attr));
        }

        uint16_t type()
        {
            return header.type;
        }

        int32_t decode(uint8_t *data, size_t len);

        size_t size(bool add_finger_print = false);

        virtual int32_t encode(uint8_t *data, size_t len, bool add_message_integrity = false, bool add_finger_print = false, const std::string &pwd = "");
    };

    //     0                   1                   2                   3
    //     0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
    //    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    //    |x x x x x x x x|    Family     |           Port                |
    //    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    //    |                             Address                           |
    //    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    struct StunMappedAddressAttr : public StunMsgAttr
    {
        StunMappedAddressAttr(uint32_t addr, uint16_t p) : StunMsgAttr(STUN_ATTR_MAPPED_ADDRESS), family(0x01)
        {
            address = addr;
            port = p;
        }

        StunMappedAddressAttr() : family(0x01)
        {
        }

        size_t size()
        {
            return StunMsgAttr::size() + 8;
        }

        int32_t encode(uint8_t *data, size_t len)
        {
            length = 8;
            uint8_t *data_start = data;
            int32_t consumed = StunMsgAttr::encode(data, len);
            if (consumed < 0)
            {
                return -1;
            }
            data += consumed;
            len -= consumed;
            if (len < 8)
            {
                return -2;
            }

            data[1] = family;
            data[0] = 0;
            data += 2;
            *(uint16_t *)data = htons(port);
            data += 2;
            *(uint32_t *)data = htonl(address);
            data += 4;
            return data - data_start;
        }

        int32_t decode(uint8_t *data, size_t len)
        {
            uint8_t *data_start = data;
            int32_t consumed = StunMsgAttr::decode(data, len);
            if (consumed < 0)
            {
                return -1;
            }
            data += consumed;
            len -= consumed;

            if (len < 8)
            {
                return -2;
            }
            family = data[1];
            data += 2;
            port = ntohs(*(uint16_t *)data);
            data += 2;
            address = ntohl(*(uint32_t *)data);
            data += 4;
            return data - data_start;
        }

        uint8_t family; // always 0x01
        uint16_t port;
        uint32_t address;
    };

    struct StunResponseAddressAttr : public StunMsgAttr
    {
        StunResponseAddressAttr() : StunMsgAttr(STUN_ATTR_RESPONSE_ADDRESS)
        {
        }

        uint8_t family; // always 0x01
        uint16_t port;
        uint32_t address;
    };

    struct StunChangeAddressAttr : public StunMsgAttr
    {
        StunChangeAddressAttr() : StunMsgAttr(STUN_ATTR_CHANGED_ADDRESS)
        {
        }

        uint8_t family; // always 0x01
        uint16_t port;
        uint32_t address;
    };

    struct StunChangeRequestAttr : public StunMsgAttr
    {
        StunChangeRequestAttr() : StunMsgAttr(STUN_ATTR_CHANGE_REQUEST)
        {
        }

        bool change_ip = false;
        bool change_port = false;
    };

    struct StunSourceAddressAttr : public StunMsgAttr
    {
        StunSourceAddressAttr() : StunMsgAttr(STUN_ATTR_SOURCE_ADDRESS)
        {
        }

        uint8_t family; // always 0x01
        uint16_t port;
        uint32_t address;
    };

    struct StunPasswordAttr : public StunMsgAttr
    {
        StunPasswordAttr() : StunMsgAttr(STUN_ATTR_PASSWORD)
        {
        }

        std::string password;
    };

    // /*
    // MESSAGE-INTEGRITY：STUN 消息的 HMAC-SHA1 值，长度 20 字节，用于消息完整性认证。详细的计算方式后续进行举例说明。
    // */
    // struct StunMessageIntegrityAttr : public StunMsgAttr
    // {
    //     StunMessageIntegrityAttr() : StunMsgAttr(STUN_ATTR_MESSAGE_INTEGRITY)
    //     {
    //     }

    //     std::string hmac_sha1;
    // };

    struct StunErrorCodeAttr : public StunMsgAttr
    {
        StunErrorCodeAttr(int32_t code, const std::string &reason) : StunMsgAttr(STUN_ATTR_ERROR_CODE)
        {
            _class = (uint8_t)(code / 100);
            number = (uint8_t)(code % 100);
        }

        size_t size()
        {
            return StunMsgAttr::size() + 4 + reason.size();
        }

        int32_t encode(uint8_t *data, size_t len)
        {
            length = 4 + reason.size();
            uint8_t *data_start = data;
            int32_t consumed = StunMsgAttr::encode(data, len);
            if (consumed < 0)
            {
                return -1;
            }
            data += consumed;
            len -= consumed;
            if (len < 4 + reason.size())
            {
                return -2;
            }

            data[3] = number;
            data[2] = 0x07 & _class;
            data[1] = 0;
            data[0] = 0;
            data += 4;
            if (reason.size() > 0)
            {
                memcpy(data, reason.data(), reason.size());
                data += reason.size();
            }

            return data - data_start;
        }

        uint8_t _class;
        uint8_t number;
        std::string reason;
    };

    struct StunUnknownAttributesAttr : public StunMsgAttr
    {
        StunUnknownAttributesAttr() : StunMsgAttr(STUN_ATTR_UNKNOWN_ATTRIBUTES)
        {
        }

        uint16_t attr[4];
    };

    struct StunReflectFromAttr : public StunMsgAttr
    {
        StunReflectFromAttr() : StunMsgAttr(STUN_ATTR_REFLECTED_FROM)
        {
        }

        uint8_t family; // always 0x01
        uint16_t port;
        uint32_t address;
    };

};
