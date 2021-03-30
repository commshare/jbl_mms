#include "system.h"

using namespace mms;
int getCPUCount() {
    int cpu_num = sysconf(_SC_NPROCESSORS_CONF);
    return cpu_num;
}