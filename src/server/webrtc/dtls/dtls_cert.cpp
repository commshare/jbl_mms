#include <iostream>
#include <sstream>
#include <iomanip>
#include <string.h>

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
    if (certificate_)
    {
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
    X509_gmtime_adj(X509_get_notBefore(certificate_), 0);              // now
    X509_gmtime_adj(X509_get_notAfter(certificate_), 365 * 24 * 3600); // accepts secs
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
    // 生成finger print
    std::unique_ptr<BIO, void (*)(BIO *)> memIO(BIO_new(BIO_s_mem()), BIO_free_all);
    auto digest = EVP_get_digestbyname("sha1");
    unsigned char md[EVP_MAX_MD_SIZE];
    unsigned int n;
    X509_digest(certificate_, digest, md, &n);
    std::ostringstream oss;
    for (unsigned int pos = 0; pos < n; pos++)
    {
        if (pos != n - 1)
        {
            oss << std::hex << std::setw(2) << std::setfill('0') << (int)md[pos] << ":";
        }
        else
        {
            oss << std::hex << std::setw(2) << std::setfill('0') << (int)md[pos];
        }
    }
    finger_print_ = oss.str();
    // 写入der
    std::unique_ptr<BIO, void (*)(BIO *)> memDER{BIO_new(BIO_s_mem()), BIO_free_all};
    ret = i2d_X509_bio(memDER.get(), certificate_);
    if (ret != 1)
    {
        return false;
    }
    char *der_ptr = NULL;
    long der_len = BIO_get_mem_data(memDER.get(), &der_ptr);
    der_.resize(der_len);
    memcpy(der_.data(), der_ptr, der_len);
    return true;
}