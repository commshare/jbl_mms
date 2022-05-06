#pragma once

#include <string>
#include <unordered_map>
// a=fmtp:<format> <format specific parameters>

// This attribute allows parameters that are specific to a
// particular format to be conveyed in a way that SDP does not
// have to understand them.  The format must be one of the formats
// specified for the media.  Format-specific parameters may be any
// set of parameters required to be conveyed by SDP and given

// Handley, et al.             Standards Track                    [Page 30]

// RFC 4566                          SDP                          July 2006

// unchanged to the media tool that will use this format.  At most
// one instance of this attribute is allowed for each format.

// It is a media-level attribute, and it is not dependent on
// charset.
namespace mms
{
    struct Fmtp
    {
    public:
        static std::string prefix;
        static bool isMyPrefix(const std::string &line);
        Fmtp() = default;
        Fmtp(int pt, const std::string &fmt_param) : pt_(pt), fmt_params_(fmt_param)
        {
        }

        bool parse(const std::string &line);

        int getPt() const
        {
            return pt_;
        }

        void setPt(int pt)
        {
            pt_ = pt;
        }
        std::string toString() const;

    public:
        int pt_; //-1代表所有pt，正值代表具体的pt
        std::string fmt_params_;
    };
};