#include "fingerprint.h"
#include <iostream>
#include "base/utils/utils.h"
using namespace mms;
std::string FingerPrint::prefix = "a=setup:";
bool FingerPrint::parse(const std::string & line) {
    std::string::size_type end_pos = line.rfind("\r");
    if (end_pos == std::string::npos) {
        end_pos = line.size() - 1;
    }
    valid_string = line.substr(prefix.size(), end_pos);
    fingerprint = valid_string;
    std::cout << "fingerprint:" << fingerprint << std::endl;
    return true;
}