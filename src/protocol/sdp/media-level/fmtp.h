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
        static std::string empty_str;
        static bool isMyPrefix(const std::string &line);
        Fmtp() = default;
        Fmtp(uint32_t pt, const std::unordered_map<std::string, std::string> & params) : pt_(pt), fmt_params_(params)
        {
        }

        bool parse(const std::string &line);

        uint32_t getPt() const
        {
            return pt_;
        }

        void setPt(uint32_t pt)
        {
            pt_ = pt;
        }
        std::string toString() const;

         std::unordered_map<std::string, std::string> & getParams() {
            return fmt_params_;
         }

         bool hasParam(const std::string & k) {
            return fmt_params_.find(k) != fmt_params_.end();
         }

         std::string getParam(const std::string & k) {
            auto it = fmt_params_.find(k);
            if (it == fmt_params_.end()) {
                return empty_str;
            }
            return it->second;
         }
    public:
        uint32_t pt_; //-1代表所有pt，正值代表具体的pt
        std::unordered_map<std::string, std::string> fmt_params_;
    };
};