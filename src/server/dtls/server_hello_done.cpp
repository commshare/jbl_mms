#include <arpa/inet.h>
#include <string.h>
#include "base/utils/utils.h"

#include "server_hello_done.h"
using namespace mms;

int32_t ServerHelloDone::decode(uint8_t *data, size_t len)
{//todo implement is
    return -1;
}

int32_t ServerHelloDone::encode(uint8_t *data, size_t len)
{
    return 0;
}

uint32_t ServerHelloDone::size()
{
    return 0;
}
