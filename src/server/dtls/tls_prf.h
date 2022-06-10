#pragma once
// refer@https://www.cryptologie.net/article/340/tls-pre-master-secrets-and-master-secrets/
// refer@doc tls1.2

#include <string>

namespace mms {
    // master_secret = PRF(pre_master_secret, "master secret",
    //                 ClientHello.random + ServerHello.random)
    //                 [0..47];
    // PRF(secret, label, seed) = P_<hash>(secret, label + seed)
    // P_hash(secret, seed) = HMAC_hash(secret, A(1) + seed) +
    //                    HMAC_hash(secret, A(2) + seed) +
    //                    HMAC_hash(secret, A(3) + seed) + ...
    // A(0) = seed
    // A(i) = HMAC_hash(secret, A(i-1))    
    // If P_256 is being used, then SHA-256 is being used. This means the output of HMAC will be 256 bits (32 bytes). To get the 48 bytes of the master key, two iterations are enough, and the remaining bytes can be discarded.
    std::string PRF(const std::string & pre_master_secret, const std::string & label, const std::string & seed, int32_t need_bytes);
};