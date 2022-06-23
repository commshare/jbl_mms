#pragma once
// 7.1.  Change Cipher Spec Protocol

//    The change cipher spec protocol exists to signal transitions in
//    ciphering strategies.  The protocol consists of a single message,
//    which is encrypted and compressed under the current (not the pending)
//    connection state.  The message consists of a single byte of value 1.

//       struct {
//           enum { change_cipher_spec(1), (255) } type;
//       } ChangeCipherSpec;

//    The ChangeCipherSpec message is sent by both the client and the
//    server to notify the receiving party that subsequent records will be
//    protected under the newly negotiated CipherSpec and keys.  Reception
//    of this message causes the receiver to instruct the record layer to
//    immediately copy the read pending state into the read current state.
//    Immediately after sending this message, the sender MUST instruct the
//    record layer to make the write pending state the write active state.



// Dierks & Rescorla           Standards Track                    [Page 27]

// RFC 5246                          TLS                        August 2008


//    (See Section 6.1.)  The ChangeCipherSpec message is sent during the
//    handshake after the security parameters have been agreed upon, but
//    before the verifying Finished message is sent.

//    Note: If a rehandshake occurs while data is flowing on a connection,
//    the communicating parties may continue to send data using the old
//    CipherSpec.  However, once the ChangeCipherSpec has been sent, the
//    new CipherSpec MUST be used.  The first side to send the
//    ChangeCipherSpec does not know that the other side has finished
//    computing the new keying material (e.g., if it has to perform a
//    time-consuming public key operation).  Thus, a small window of time,
//    during which the recipient must buffer the data, MAY exist.  In
//    practice, with modern machines this interval is likely to be fairly
//    short.
#include <stdint.h>
#include "dtls_define.h"
namespace mms {
struct ChangeCipherSpec : public DtlsMsg {
    uint8_t type = 0x01;
    int32_t decode(uint8_t *data, size_t len);
    int32_t encode(uint8_t *data, size_t len);
    uint32_t size();
};
};