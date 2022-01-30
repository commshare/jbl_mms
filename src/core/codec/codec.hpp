#pragma once
#include <string>
namespace mms {
enum CodecType {
    CODEC_H264  = 0,
    CODEC_HEVC  = 1,
    CODEC_AAC   = 2,
};

class Codec {
public:
    std::string & getCodecName() {
        return codec_name_;
    }

    CodecType getCodecType() {
        return codec_type_;
    }
private:
    CodecType   codec_type_;
    std::string codec_name_;
};

};