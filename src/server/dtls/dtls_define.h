#pragma once
#include <stdint.h>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace mms
{
    enum ContentType
    {
        change_cipher_spec = 20,
        alert = 21,
        handshake = 22,
        application_data = 23
    };

#define DTLS_MAJOR_VERSION1 0xfe

#define DTLS_MINOR_VERSION0 0xff
#define DTLS_MINOR_VERSION2 0xfd
#define DTLS_MINOR_VERSION1 0xfe

    struct DtlsProtocolVersion
    {
        DtlsProtocolVersion() = default;
        DtlsProtocolVersion(uint8_t ma, uint8_t mi)
        {
            major = ma;
            minor = mi;
        }

        bool operator==(const DtlsProtocolVersion &b) const
        {
            if (major == b.major && minor == b.minor)
            {
                return true;
            }
            return false;
        }

        bool operator!=(const DtlsProtocolVersion &b) const
        {
            if (major == b.major && minor == b.minor)
            {
                return false;
            }
            return true;
        }

        uint8_t major, minor;
        int32_t decode(uint8_t *data, size_t len);
        int32_t encode(uint8_t *data, size_t len);
        uint32_t size()
        {
            return 2;
        }
    };

    struct Random
    {
        uint32_t gmt_unix_time;
        uint8_t random_bytes[28];
        uint8_t random_raw[32];
        int32_t decode(uint8_t *data, size_t len);
        int32_t encode(uint8_t *data, size_t len);
        uint32_t size()
        {
            return 32;
        }
        void genRandom();
    };

    #define DTLS_HEADER_SIZE 13
    struct DtlsHeader
    {
        ContentType type;             /* same as TLSPlaintext.type */
        DtlsProtocolVersion version;  /* same as TLSPlaintext.version */
        uint16_t epoch;               // New field
        uint64_t sequence_number = 1; // New field(48bit)
        uint16_t length;
        int32_t decode(uint8_t *data, size_t len);
        int32_t encode(uint8_t *data, size_t len);
        uint32_t size()
        {
            return DTLS_HEADER_SIZE;
        }
    };

    struct DtlsMsg
    {
        virtual int32_t decode(uint8_t *data, size_t len) = 0;
        virtual int32_t encode(uint8_t *data, size_t len) = 0;
        virtual uint32_t size() = 0;
    };

    typedef uint16_t CipherSuite;
    // 加密套件定义
    #define TLS_RSA_EXPORT_WITH_RC4_40_MD5 0x0003
    #define TLS_RSA_WITH_RC4_128_MD5 0x0004
    #define TLS_RSA_WITH_RC4_128_SHA 0x0005
    #define TLS_DH_anon_EXPORT_WITH_RC4_40_MD5 0x0017
    #define TLS_DH_anon_WITH_RC4_128_MD5 0x0018
    #define TLS_KRB5_WITH_RC4_128_SHA 0x0020
    #define TLS_KRB5_WITH_RC4_128_MD5 0x0024
    #define TLS_KRB5_EXPORT_WITH_RC4_40_SHA 0x0028
    #define TLS_KRB5_EXPORT_WITH_RC4_40_MD5 0x002B
    #define TLS_PSK_WITH_RC4_128_SHA 0x008A
    #define TLS_DHE_PSK_WITH_RC4_128_SHA 0x008E
    #define TLS_RSA_PSK_WITH_RC4_128_SHA 0x0092
    #define TLS_ECDH_ECDSA_WITH_RC4_128_SHA 0xC002
    #define TLS_ECDHE_ECDSA_WITH_RC4_128_SHA 0xC007
    #define TLS_ECDH_RSA_WITH_RC4_128_SHA 0xC00C
    #define TLS_ECDHE_RSA_WITH_RC4_128_SHA 0xC011
    #define TLS_ECDH_anon_WITH_RC4_128_SHA 0xC016
    #define TLS_ECDHE_PSK_WITH_RC4_128_SHA 0xC033

    #define TLS_RSA_WITH_NULL_MD5 0x0001
    #define TLS_RSA_WITH_NULL_SHA 0x0002
    #define TLS_RSA_WITH_NULL_SHA256 0x003B
    #define TLS_RSA_WITH_3DES_EDE_CBC_SHA 0x000A
    #define TLS_RSA_WITH_AES_128_CBC_SHA 0x002F
    #define TLS_RSA_WITH_AES_256_CBC_SHA 0x0035
    #define TLS_RSA_WITH_AES_128_CBC_SHA256 0x003C
    #define TLS_RSA_WITH_AES_256_CBC_SHA256 0x003D
    #define TLS_DH_DSS_WITH_3DES_EDE_CBC_SHA 0x000D
    #define TLS_DH_RSA_WITH_3DES_EDE_CBC_SHA 0x0010
    #define TLS_DHE_DSS_WITH_3DES_EDE_CBC_SHA 0x0013
    #define TLS_DHE_RSA_WITH_3DES_EDE_CBC_SHA 0x0016
    #define TLS_DH_DSS_WITH_AES_128_CBC_SHA 0x0030
    #define TLS_DH_RSA_WITH_AES_128_CBC_SHA 0x0031
    #define TLS_DHE_DSS_WITH_AES_128_CBC_SHA 0x0032
    #define TLS_DHE_RSA_WITH_AES_128_CBC_SHA 0x0033
    #define TLS_DH_DSS_WITH_AES_256_CBC_SHA 0x0036
    #define TLS_DH_RSA_WITH_AES_256_CBC_SHA 0x0037
    #define TLS_DHE_DSS_WITH_AES_256_CBC_SHA 0x0038
    #define TLS_DHE_RSA_WITH_AES_256_CBC_SHA 0x0039
    #define TLS_DH_DSS_WITH_AES_128_CBC_SHA256 0x003E
    #define TLS_DH_RSA_WITH_AES_128_CBC_SHA256 0x003F
    #define TLS_DHE_DSS_WITH_AES_128_CBC_SHA256 0x0040
    #define TLS_DHE_RSA_WITH_AES_128_CBC_SHA256 0x0067
    #define TLS_DH_DSS_WITH_AES_256_CBC_SHA256 0x0068
    #define TLS_DH_RSA_WITH_AES_256_CBC_SHA256 0x0069
    #define TLS_DHE_DSS_WITH_AES_256_CBC_SHA256 0x006A
    #define TLS_DHE_RSA_WITH_AES_256_CBC_SHA256 0x006B
    #define TLS_DH_anon_WITH_RC4_128_MD5 0x0018
    #define TLS_DH_anon_WITH_3DES_EDE_CBC_SHA 0x001B
    #define TLS_DH_anon_WITH_AES_128_CBC_SHA 0x0034
    #define TLS_DH_anon_WITH_AES_256_CBC_SHA 0x003A
    #define TLS_DH_anon_WITH_AES_128_CBC_SHA256 0x006C
    #define TLS_DH_anon_WITH_AES_256_CBC_SHA256 0x006D

    struct CipherSuites
    {
        std::vector<CipherSuite> cipher_suites;
        int32_t decode(uint8_t *data, size_t len);
        uint32_t size();
    };

    typedef uint8_t CompressionMethod;
#define COMPRESSION_METHOD_NULL 0x00
    struct CompressionMethods
    {
        std::vector<CompressionMethod> compression_methods;
        int32_t decode(uint8_t *data, size_t len);
        uint32_t size();
    };

    struct GenericBlockCipher : public DtlsMsg
    {
        std::string IV;//uint8_t IV[SecurityParameters.record_iv_length];//record_iv_length equal to block_size
        //以下数据加密, 生成消息体
        struct BlockCipered 
        {
            std::string content;//uint8_t content[];
            std::string MAC;    //uint8_t[20]; //本例使用HMAC-SHA1, 输出20字节
            std::string padding;
            uint8_t padding_length;
            // uint8 padding[GenericBlockCipher.padding_length]; //用于对齐16字节. 填充的内容为padding_length
            // uint8 padding_length;  //对齐字节的长度，最终整个个结构体必须是16的倍数.
        };
        struct BlockCipered block_cipered;
    };
    
    struct DTLSPlaintext
    {
        DtlsHeader header;
        std::unique_ptr<DtlsMsg> msg;
        DTLSPlaintext() = default;
        DTLSPlaintext(DTLSPlaintext &other)
        {
            header = other.header;
            msg = std::move(other.msg);
        }

        DTLSPlaintext &operator=(DTLSPlaintext &other)
        {
            header = other.header;
            msg = std::move(other.msg);
            return *this;
        }

        ContentType getType() const
        {
            return header.type;
        }

        void setType(ContentType v)
        {
            header.type = v;
        }

        void setDtlsProtocolVersion(const DtlsProtocolVersion &v)
        {
            header.version = v;
        }

        void setMsg(std::unique_ptr<DtlsMsg> val)
        {
            msg = std::move(val);
        }

        void setSequenceNo(uint32_t val)
        {
            header.sequence_number = val;
        }

        uint64_t getSequenceNo()
        {
            return header.sequence_number;
        }


        int32_t decode(uint8_t *data, size_t len, bool cipered = false);
        int32_t encode(uint8_t *data, size_t len);
        uint32_t size()
        {
            return header.size() + msg->size();
        }
    };

    // @doc https://datatracker.ietf.org/doc/html/rfc5246#page-44
    //    The presence of extensions can be detected by
    //    determining whether there are bytes following the compression_methods
    //    at the end of the ClientHello.  Note that this method of detecting
    //    optional data differs from the normal TLS method of having a
    //    variable-length field, but it is used for compatibility with TLS
    //    before extensions were defined.
    enum ExtensionType
    {
        signature_algorithms = 0x000D,
        use_srtp = 0x000E
    };

    struct DtlsExtensionHeader
    {
        ExtensionType type;
        uint16_t length;
        int32_t decode(uint8_t *data, size_t len);
        int32_t encode(uint8_t *data, size_t len);
        uint32_t size()
        {
            return 4;
        }
    };

    struct DtlsExtItem
    {
        DtlsExtensionHeader header;
        ExtensionType getType() const
        {
            return header.type;
        }

        virtual int32_t decode(uint8_t *data, size_t len) = 0;
        virtual int32_t encode(uint8_t *data, size_t len) = 0;
        virtual uint32_t size() = 0;
    };

    struct UnknownExtItem : public DtlsExtItem
    {
        int32_t decode(uint8_t *d, size_t len);
        int32_t encode(uint8_t *d, size_t len);
        uint32_t size()
        {
            return 0;
        }
    };

    struct DtlsExtension
    {
        std::unordered_map<ExtensionType, std::unique_ptr<DtlsExtItem>> extensions;
        int32_t decode(uint8_t *data, size_t len);
        int32_t encode(uint8_t *data, size_t len);
        uint32_t size();

        DtlsExtItem *getExtension(ExtensionType t)
        {
            auto it = extensions.find(t);
            if (it == extensions.end())
            {
                return nullptr;
            }
            return it->second.get();
        }

        void addExtension(std::unique_ptr<DtlsExtItem> ext_item)
        {
            extensions.insert(std::pair(ext_item->getType(), std::move(ext_item)));
        }
    };

    typedef enum
    {
        server,
        client
    } ConnectionEnd;

    typedef enum
    {
        tls_prf_sha256
    } PRFAlgorithm;

    typedef enum
    {
        null_b,
        rc4,
        des3,
        aes
    } BulkCipherAlgorithm;

    typedef enum
    {
        stream,
        block,
        aead
    } CipherType;

    typedef enum
    {
        null_m,
        hmac_md5,
        hmac_sha1,
        hmac_sha256,
        hmac_sha384,
        hmac_sha512
    } MACAlgorithm;

    struct SecurityParameters
    {
        ConnectionEnd entity = server;
        PRFAlgorithm prf_algorithm = tls_prf_sha256;
        BulkCipherAlgorithm bulk_cipher_algorithm;
        CipherType cipher_type = block;
        uint8_t enc_key_length;
        uint8_t block_length;
        uint8_t fixed_iv_length;
        uint8_t record_iv_length;
        MACAlgorithm mac_algorithm;
        uint8_t mac_length;
        uint8_t mac_key_length;
        CompressionMethod compression_algorithm;
        uint8_t master_secret[48];
        uint8_t client_random[32];
        uint8_t server_random[32];
    };

    //                       Key       IV    Block
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

    enum CipherSuiteKeyExchangeAlgorithm {
        CipherSuiteKeyExchangeAlgorithm_RSA, 
        CipherSuiteKeyExchangeAlgorithm_DHE_RSA,
        CipherSuiteKeyExchangeAlgorithm_DH_RSA, 
        CipherSuiteKeyExchangeAlgorithm_RSA_PSK, 
        CipherSuiteKeyExchangeAlgorithm_ECDH_RSA, 
        CipherSuiteKeyExchangeAlgorithm_ECDHE_RSA
    };

    struct CiperSuite
    {
        uint16_t ciper_id;
        CipherType ciper_type;
        uint8_t mac_length;
        uint8_t mac_key_length;
        uint8_t record_iv_length;
        uint8_t enc_key_length;
        CipherSuiteKeyExchangeAlgorithm key_exchange_algorithm;
        
        bool initialized = false;
        std::string client_write_MAC_key;
        std::string server_write_MAC_key;
        std::string client_write_key;
        std::string server_write_key;
        std::string client_write_IV;
        std::string server_write_IV;

        // virtual void init(const std::string & master_secret, const std::string & client_random, const std::string & server_random, bool is_client);

        // virtual int32_t decrypt(const std::string & in, std::string & out);
        // virtual int32_t encrypt(const std::string & in, std::string & out);
    };
};