#pragma once 
#include <string>  
namespace mms
{
    struct RtcpMux
    {
    public:
        static std::string prefix;
        bool parse(const std::string &line);
        std::string toString() const;
    };
} // namespace mms;
