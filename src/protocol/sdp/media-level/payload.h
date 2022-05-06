#pragma once
#include <string>
#include <vector>
#include "rtcp_fb.h"
#include "fmtp.h"

namespace mms
{
    struct Payload
    {
    public:
        static std::string prefix;
        static bool isMyPrefix(const std::string &line);
        bool parse(const std::string &line);
        Payload() = default;
        Payload(uint32_t payload_type, const std::string &encoding_name, uint32_t clock_rate, const std::vector<std::string> &encoding_params) : payload_type_(payload_type), encoding_name_(encoding_name), clock_rate_(clock_rate), encoding_params_(encoding_params)
        {
        }
        bool parseRtcpFbAttr(const std::string &line);
        bool parseFmtpAttr(const std::string &line);

        const std::vector<RtcpFb> &getRtcpFbs()
        {
            return rtcp_fbs_;
        }

        void setRtcpFbs(const std::vector<RtcpFb> &rtcp_fbs)
        {
            rtcp_fbs_ = rtcp_fbs;
        }

        void addRtcpFb(const RtcpFb &rtcp_fb)
        {
            rtcp_fbs_.push_back(rtcp_fb);
        }

        const std::string &getEncodingName() const
        {
            return encoding_name_;
        }

        uint32_t getClockRate() const
        {
            return clock_rate_;
        }

        uint32_t getPt() const
        {
            return payload_type_;
        }

        void setPt(uint32_t pt)
        {
            payload_type_ = pt;
        }

        void addFmtp(const Fmtp &fmtp)
        {
            fmtps_.insert(std::pair(fmtp.getPt(), fmtp));
        }

        const std::unordered_map<int, Fmtp> &getFmtps() const
        {
            return fmtps_;
        }

        const std::vector<std::string> & getEncodingParams() const {
            return encoding_params_;
        }

        std::string toString() const;

    private:
        uint32_t payload_type_;
        std::string encoding_name_;
        uint32_t clock_rate_;
        std::vector<std::string> encoding_params_;
        std::vector<RtcpFb> rtcp_fbs_;
        std::unordered_map<int, Fmtp> fmtps_;
    };
};