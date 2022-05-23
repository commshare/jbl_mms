#include <iostream>
#include "dtls_cert.h"
using namespace mms;

bool DtlsCert::init(const std::string &sign_method)
{
    sign_method_ = sign_method;
    if (!createCert())
    {
        return false;
    }

    return true;
}

bool DtlsCert::createCert()
{
    // 创建私钥
    std::srand(time(nullptr));
    int seq = std::rand() % 9999999;

    // Step1, 创建ECKEY
    EC_KEY *ec_key = EC_KEY_new_by_curve_name(NID_X9_62_prime256v1);
    if (!ec_key)
    {
        return false;
    }
    EC_KEY_set_asn1_flag(ec_key, OPENSSL_EC_NAMED_CURVE);

    // 这里初始化生成时，可能需要一定的时间
    int ret = EC_KEY_generate_key(ec_key);
    if (ret == 0)
    {
        return false;
    }

    // Step2，创建私Key
    private_key_ = EVP_PKEY_new();
    if (!private_key_)
    {
        return false;
    }

    // Step3, 绑定私Key和ECKEY后，ECKEY不需要自己释放，所以可以直接置空
    ret = EVP_PKEY_assign_EC_KEY(private_key_, ec_key);
    if (ret == 0)
    {
        return false;
    }
    ec_key = NULL;

    // Step4，创建证书
    certificate_ = X509_new();
    if (!certificate_)
    {
        std::cout << "X509_new failed" << std::endl;
        return false;
    }

    X509_set_version(certificate_, 2);

    ASN1_INTEGER_set(X509_get_serialNumber(certificate_), seq);

    static const int expired_days = 365 * 10;
    static const long int expired_time_after = 60 * 60 * 24 * expired_days;
    static const long int expired_time_before = -1 * expired_days;
    X509_gmtime_adj(X509_get_notBefore(certificate_), expired_time_before);
    X509_gmtime_adj(X509_get_notAfter(certificate_), expired_time_after);

    ret = X509_set_pubkey(certificate_, private_key_);
    if (ret == 0)
    {
        std::cout << "X509_set_pubkey failed" << std::endl;
        return false;
    }

    // Step5, 生成证书信息
    X509_NAME *subject_name = X509_get_subject_name(certificate_);
    if (!subject_name)
    {
        return false;
    }

    std::string subject = "mms_" + std::to_string(seq);
    X509_NAME_add_entry_by_txt(subject_name, "O", MBSTRING_ASC,
                               (const unsigned char *)subject.c_str(), subject.size(), -1, 0);

    static const std::string &domain = "mms.cn";
    X509_NAME_add_entry_by_txt(subject_name, "CN", MBSTRING_ASC,
                               (const unsigned char *)domain.c_str(), domain.size(), -1, 0);

    ret = X509_set_issuer_name(certificate_, subject_name);
    if (ret == 0)
    {
        std::cout << "X509_set_issuer_name failed" << std::endl;
        return false;
    }

    // Step6, 生成证书签名
    ret = X509_sign(certificate_, private_key_, EVP_sha1());
    if (ret == 0)
    {
        std::cout << "X509_sign failed" << std::endl;
        return false;
    }

    std::cout << "init default certs ok" << std::endl;
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
