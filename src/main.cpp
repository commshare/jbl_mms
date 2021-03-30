#include <iostream>
#include "system/system.h"

int main(char argc, char *argv[]) {
    std::cout << "cpu count:" << mms::getCPUCount() << std::endl;
    return 0;
}