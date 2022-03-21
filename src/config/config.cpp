#include "config.h"

using namespace mms;
Config Config::instance_;

Config & Config::getInstance() {
    return instance_;
}

bool Config::loadFromFile(const std::string & config_file) {
    return false;
}