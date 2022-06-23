#include "change_cipher_spec.h"

#include <iostream>
using namespace mms;

int32_t ChangeCipherSpec::encode(uint8_t *data, size_t len)
{//todo
    if (len < 1) {
        return -1;
    }
    *data = type;
    return 1;
}

int32_t ChangeCipherSpec::decode(uint8_t *data, size_t len)
{
    if (len < 1) {
        return -1;
    }
    type = *data;
    return 1;
}

uint32_t ChangeCipherSpec::size()
{
    return 1;
}