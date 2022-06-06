#include <iostream>
#include <sstream>
#include <iomanip>
#include "dtls_cert.h"
using namespace mms;
DtlsCert DtlsCert::instance_;
bool DtlsCert::init()
{
    if (!createCert())
    {
        return false;
    }

    return true;
}

DtlsCert::~DtlsCert()
{
    if (certificate_) {
        X509_free(certificate_);
        certificate_ = nullptr;
    }
}

#define RSA_KEY_LENGTH 512
bool DtlsCert::createCert()
{
    SSL_load_error_strings();
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    // 创建证书
    certificate_ = X509_new();
    // 生成随机序号
    std::srand(time(nullptr));
    int seq = std::rand() % 9999999;
    ASN1_INTEGER_set(X509_get_serialNumber(certificate_), seq); // serial number
    // 创建pkey
    std::unique_ptr<RSA, void (*)(RSA *)> rsa{RSA_new(), RSA_free};
    std::unique_ptr<BIGNUM, void (*)(BIGNUM *)> bn{BN_new(), BN_free};
    std::unique_ptr<EVP_PKEY, void (*)(EVP_PKEY *)> pkey{EVP_PKEY_new(), EVP_PKEY_free};
    BN_set_word(bn.get(), RSA_F4);
    int ret = RSA_generate_key_ex(rsa.get(), RSA_KEY_LENGTH, bn.get(), nullptr);
    if (ret != 1)
    {
        return false;
    }
    EVP_PKEY_assign(pkey.get(), EVP_PKEY_RSA, reinterpret_cast<char *>(rsa.release()));
    X509_set_pubkey(certificate_, pkey.get());
    // 设置时长
    X509_gmtime_adj(X509_get_notBefore(certificate_), 0);                    // now
    X509_gmtime_adj(X509_get_notAfter(certificate_), 365 * 24 * 3600);    // accepts secs
    // 设置域名等信息
    // 1 -- X509_NAME may disambig with wincrypt.h
    // 2 -- DO NO FREE the name internal pointer
    X509_name_st *name = X509_get_subject_name(certificate_);
    const u_char country[] = "China";
    const u_char company[] = "mms, PLC";
    const u_char common_name[] = "mms.cn";
    X509_NAME_add_entry_by_txt(name, "C", MBSTRING_ASC, country, -1, -1, 0);
    X509_NAME_add_entry_by_txt(name, "O", MBSTRING_ASC, company, -1, -1, 0);
    X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC, common_name, -1, -1, 0);
    X509_set_issuer_name(certificate_, name);
    // 生成签名
    ret = X509_sign(certificate_, pkey.get(), EVP_sha1()); // some hash type here
    if (ret == 0) 
    {
        return false;
    }
    // // 生成finger print
    std::unique_ptr<BIO, void (*)(BIO *)> memIO(BIO_new(BIO_s_mem()), BIO_free_all);
    auto digest = EVP_get_digestbyname("sha1");
    unsigned char md[EVP_MAX_MD_SIZE];
    unsigned int n;
    X509_digest(certificate_, digest, md, &n);
    std::ostringstream oss;
    for (int pos = 0; pos < n; pos++) {
        if (pos != n - 1) {
            oss << std::hex << std::setw(2) << std::setfill('0') << (int)md[pos] << ":";
        } else {
            oss << std::hex << std::setw(2) << std::setfill('0') << (int)md[pos];
        }
    }
    finger_print_ = oss.str();
    std::cout << "fingerprint=" << finger_print_ << std::endl;
    return true;
}

bool DtlsCert::initSSLCtx()
{

    // requires OpenSSL >= 1.1.0
    // 如果是低版本的OpenSSL, 这里创建时需要关注client, server
    ssl_ctx_ = SSL_CTX_new(DTLS_method());
    if (!ssl_ctx_)
    {
        std::cout << "SSL_CTX_new failed." << std::endl;
        return false;
    };

    // SSL_CTX_use_certificate返回1为成功，0为失败
    int ret = SSL_CTX_use_certificate(ssl_ctx_, certificate_);
    if (ret == 0)
    {
        std::cout << "SSL_CTX_use_certificate failed, ret:" << ret << std::endl;
        return false;
    }

    ret = SSL_CTX_use_PrivateKey(ssl_ctx_, private_key_);
    if (ret == 0)
    {
        std::cout << "SSL_CTX_use_certificate failed, ret:" << ret << std::endl;
        return false;
    }

    ret = SSL_CTX_check_private_key(ssl_ctx_);
    if (ret == 0)
    {
        std::cout << "SSL_CTX_check_private_key failed, ret:" << ret << std::endl;
        return false;
    }

    SSL_CTX_set_options(ssl_ctx_,
                        SSL_OP_CIPHER_SERVER_PREFERENCE | SSL_OP_NO_TICKET | SSL_OP_SINGLE_ECDH_USE | SSL_OP_NO_QUERY_MTU);

    SSL_CTX_set_session_cache_mode(ssl_ctx_, SSL_SESS_CACHE_OFF);

    SSL_CTX_set_read_ahead(ssl_ctx_, 1);

    SSL_CTX_set_verify_depth(ssl_ctx_, 4);

    // 设置SSL的验证方式，回调处理实际上服务端不做验证
    SSL_CTX_set_verify(ssl_ctx_, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, DtlsCert::sslVerifyCallback);

    // 回调处理SSL相关调试信息，及确定客户端还是服务端
    SSL_CTX_set_info_callback(ssl_ctx_, DtlsCert::sslInfo);

    // 默认打开所有的支持
    // ret = SSL_CTX_set_cipher_list(ssl_ctx_, "ALL");
    ret = SSL_CTX_set_cipher_list(ssl_ctx_, "DEFAULT:!NULL:!aNULL:!SHA256:!SHA384:!aECDH:!AESGCM+AES256:!aPSK");

    if (ret == 0)
    {
        std::cout << "SSL_CTX_set_cipher_list failed, ret:" << ret << std::endl;
        return false;
    }

#if OPENSSL_VERSION_NUMBER < 0x10100000L
    // 我们使用BoringSSL 0x1010007fL, 实际上不需要这个函数
    SSL_CTX_set_ecdh_auto(ssl_ctx_, 1);
#endif

    /*
    #define SRTP_AES128_CM_SHA1_80 0x0001
    #define SRTP_AES128_CM_SHA1_32 0x0002
    #define SRTP_AES128_F8_SHA1_80 0x0003
    #define SRTP_AES128_F8_SHA1_32 0x0004
    #define SRTP_NULL_SHA1_80      0x0005
    #define SRTP_NULL_SHA1_32      0x0006
    #define SRTP_AEAD_AES_128_GCM  0x0007
    #define SRTP_AEAD_AES_256_GCM  0x0008
    */

    // static const std::string srtp_profile
    //     = "SRTP_AEAD_AES_256_GCM:SRTP_AEAD_AES_128_GCM:SRTP_NULL_SHA1_32:SRTP_NULL_SHA1_80:"
    //       "SRTP_AES128_F8_SHA1_32:SRTP_AES128_F8_SHA1_80:SRTP_AES128_CM_SHA1_32:SRTP_AES128_CM_SHA1_80"
    //       ;
    static const std::string srtp_profile = "SRTP_AES128_CM_SHA1_80";

    ret = SSL_CTX_set_tlsext_use_srtp(ssl_ctx_, srtp_profile.c_str());
    // SSL_CTX_set_tlsext_use_srtp 返回0表示成功，1表示出错
    if (ret == 1)
    {
        std::cout << "SSL_CTX_set_tlsext_use_srtp failed, ret:" << ret << std::endl;
    }

    return true;
}

int DtlsCert::sslVerifyCallback(int preverify_ok, X509_STORE_CTX *ctx)
{
    // 做为服务端，我们不验证客户端的证书
    return 1;
}

void DtlsCert::sslInfo(const SSL *ssl, int where, int ret)
{
    // uv_dtls_layer *layer = static_cast<uv_dtls_layer *>(SSL_get_ex_data(ssl, 0));
    // int w = where & -SSL_ST_MASK;

    // if (w & SSL_ST_CONNECT)
    // {
    //     layer->set_client_mode(true);
    // }
    // else if (w & SSL_ST_ACCEPT)
    // {
    //     layer->set_client_mode(false);
    // }
    // else
    // {
    //     // 我们默认就是服务器
    //     layer->set_client_mode(false);
    // }

    // if (where & SSL_CB_LOOP)
    // {
    //     UV_RTC_UTRACE(layer->user()) << "state:" << SSL_state_string_long(ssl);
    // }
    // else if (where & SSL_CB_ALERT)
    // {
    //     if (where & SSL_CB_READ)
    //     {
    //         UV_RTC_UTRACE(layer->user()) << "on read, info:" << SSL_alert_type_string(ret) << ", " << SSL_alert_desc_string_long(ret);
    //     }
    //     else if (where & SSL_CB_WRITE)
    //     {
    //         UV_RTC_UTRACE(layer->user()) << "on write, info:" << SSL_alert_type_string(ret) << ", " << SSL_alert_desc_string_long(ret);
    //     }
    //     else
    //     {
    //         UV_RTC_UTRACE(layer->user()) << "ssl info:" << SSL_alert_type_string(ret) << ", " << SSL_alert_desc_string_long(ret);
    //     }
    // }
    // else if (where & SSL_CB_EXIT)
    // {
    //     UV_RTC_UTRACE(layer->user()) << "exit state:" << SSL_state_string_long(ssl);
    // }
    // else if (where & SSL_CB_HANDSHAKE_START)
    // {
    //     UV_RTC_UTRACE(layer->user()) << "handshake begin.";
    // }
    // else if (where & SSL_CB_HANDSHAKE_DONE)
    // {
    //     layer->state_ = server_done_state;
    //     UV_RTC_UTRACE(layer->user()) << "handshake done.";
    // }
}
