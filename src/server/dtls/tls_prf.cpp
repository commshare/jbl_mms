#include "tls_prf.h"

#include "base/utils/utils.h"

using namespace mms;

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
std::string mms::PRF(const std::string & secret, const std::string & label, const std::string & seed, int32_t need_bytes)
{
    int32_t left = need_bytes;
    std::string A0 = secret;
    std::string Ai0 = Utils::calcHmacSHA256(secret, A0 + label + seed);
    std::string out = Ai0;
    while (left > 0) {
        std::string Ai1 = Utils::calcHmacSHA256(secret, Ai0 + label + seed);
        out += Ai1;
        Ai0 = Ai1;
        left -= 32;
    }

    return out.substr(0, need_bytes);
}