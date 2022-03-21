#pragma once
#include <string_view>
#include <string>
#include <vector>
// a=rtpmap:<payload type> <encoding name>/<clock rate> [/<encoding
//          parameters>]

//          This attribute maps from an RTP payload type number (as used in
//          an "m=" line) to an encoding name denoting the payload format
//          to be used.  It also provides information on the clock rate and
//          encoding parameters.  It is a media-level attribute that is not
//          dependent on charset.
// 这个属性自定义payloadtype和编码器及参数的对应关系;
// 如果没有，则需要参考对应的文档的固定payloadtype和编码器对应关系表;
namespace mms {
struct RTPMapAttr {
static std::string prefix = "a=recvonly";
public:
};
};