#include "session_sdp.hpp"
using namespace mms;
// v=0
// o=- 825758117577907218 2 IN IP4 127.0.0.1
// s=-
// t=0 0
// a=group:BUNDLE 0 1
// a=extmap-allow-mixed
// a=msid-semantic: WMS cvej17Al75ahrQExZiyZ57GOIZiDr8E132fx
int32_t SessionDescription::read(uint8_t *data, size_t len) {
    std::string sdata((const char*)data, len);
    size_t pos = 0;
    if ((pos = sdata.find('\n', pos)) == std::string::npos) {
        return 0;
    }
    if (pos >= 1) {
        if (sdata[pos - 1] == '\r') {
            
        }
    }
    
}