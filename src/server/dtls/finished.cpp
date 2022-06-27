#include <iostream>
#include <string.h>

#include "finished.h"
using namespace mms;

int32_t DtlsFinished::decode(uint8_t *data, size_t len)
{
    uint8_t *data_start = data;
    verify_data.assign((char*)data, len);
    data += len;
    return data - data_start;
}

int32_t DtlsFinished::encode(uint8_t *data, size_t len)
{
    memcpy(data, verify_data.data(), 12);
    return 12;
}