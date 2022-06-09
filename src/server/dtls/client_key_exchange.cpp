// #include <arpa/inet.h>
// #include <iostream>

// #include "client_key_exchange.h"
// using namespace mms;

// int32_t EncryptedPreMasterSecret::decode(uint8_t *data, size_t len)
// {
//     pre_master_secret.assign((char*)data, len);
//     return len;
// }

// int32_t EncryptedPreMasterSecret::encode(uint8_t *data, size_t len)
// {
//     return 0;
// }

// int32_t ClientKeyExchange::decode(uint8_t *data, size_t len)
// {
//     uint8_t *data_start = data;
//     if (len < 2)
//     {
//         return -1;
//     }
//     uint16_t encrypted_premaster_len = ntohs(*(uint16_t*)data);
//     data += 2;
//     len -= 2;
//     if (len < encrypted_premaster_len)
//     {
//         return -2;
//     }

//     int32_t consumed = enc_pre_master_secret.decode(data, encrypted_premaster_len);
//     if (consumed < 0)
//     {
//         return -3;
//     }
//     data += consumed;
//     len -= consumed;

//     return data - data_start;
// }

// int32_t ClientKeyExchange::encode(uint8_t *data, size_t len)
// {//todo:implement it
//     return 0;
// }