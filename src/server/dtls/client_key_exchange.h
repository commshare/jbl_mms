// #pragma once
// // 7.4.7.  Client Key Exchange Message

// //    When this message will be sent:

// //       This message is always sent by the client.  It MUST immediately
// //       follow the client certificate message, if it is sent.  Otherwise,
// //       it MUST be the first message sent by the client after it receives
// //       the ServerHelloDone message.

// //    Meaning of this message:

// //       With this message, the premaster secret is set, either by direct
// //       transmission of the RSA-encrypted secret or by the transmission of
// //       Diffie-Hellman parameters that will allow each side to agree upon
// //       the same premaster secret.

// //       When the client is using an ephemeral Diffie-Hellman exponent,
// //       then this message contains the client's Diffie-Hellman public
// //       value.  If the client is sending a certificate containing a static
// //       DH exponent (i.e., it is doing fixed_dh client authentication),
// //       then this message MUST be sent but MUST be empty.

// //    Structure of this message:

// //       The choice of messages depends on which key exchange method has
// //       been selected.  See Section 7.4.3 for the KeyExchangeAlgorithm
// //       definition.

// // Dierks & Rescorla           Standards Track                    [Page 57]

// // RFC 5246                          TLS                        August 2008


// //       struct {
// //           select (KeyExchangeAlgorithm) {
// //               case rsa:
// //                   EncryptedPreMasterSecret;
// //               case dhe_dss:
// //               case dhe_rsa:
// //               case dh_dss:
// //               case dh_rsa:
// //               case dh_anon:
// //                   ClientDiffieHellmanPublic;
// //           } exchange_keys;
// //       } ClientKeyExchange;
// #include <string>
// #include <stdint.h>

// #include "dtls_define.h"
// #include "dtls_handshake.h"

// namespace mms {
//     struct PreMasterSecret {
//         // Note: The version number in the PreMasterSecret is the version
//         // offered by the client in the ClientHello.client_version, not the
//         // version negotiated for the connection.  This feature is designed to
//         // prevent rollback attacks.  Unfortunately, some old implementations
//         // use the negotiated version instead, and therefore checking the
//         // version number may lead to failure to interoperate with such
//         // incorrect client implementations.
//         DtlsProtocolVersion client_version;
//         uint8_t random[46];
//     };

//     struct EncryptedPreMasterSecret {
//         std::string pre_master_secret;
//         int32_t decode(uint8_t *data, size_t len);
//         int32_t encode(uint8_t *data, size_t len);
//         uint32_t size() {//todo:implement this later
//             return 0;
//         }
//     };

//     struct ClientKeyExchange : public HandShakeMsg {// note:we only use rsa key exchange algorithm, so we only need to deal with rsa
//         EncryptedPreMasterSecret enc_pre_master_secret;
//         int32_t decode(uint8_t *data, size_t len);
//         int32_t encode(uint8_t *data, size_t len);
//         uint32_t size() {//todo:implement this
//             return 0;
//         }
//     };
// };