#pragma once
#include <stdint.h>
#include <string>
#include <string>

namespace mms {
struct ProtocolVersion {
public:
    static std::string prefix;
    bool parse(const std::string & line);
    int32_t getVersion();
    void setVersion(int v);
    std::string toString() const;
private:
    int version;
};
};