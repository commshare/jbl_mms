#pragma once
#include <string>
namespace mms {
class Config {
public:
    Config & getInstance();
    bool loadFromFile(const std::string & config_file);
private:
    static Config instance_;
};
};