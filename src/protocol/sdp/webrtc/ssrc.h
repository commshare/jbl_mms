#pragma once
#include <string>
namespace mms {
struct Ssrc {
public:
    static std::string prefix;
    Ssrc() = default;
    Ssrc(const Ssrc & s) {
        id_ = s.id_;
        cname_ = s.cname_;
        mslabel_ = s.mslabel_;
        label_ = s.label_;
    }

    Ssrc(uint32_t id, const std::string & cname, const std::string & mslabel, const std::string & label) {
        id_ = id;
        cname_ = cname;
        mslabel_ = mslabel;
        label_ = label;
    }

    static uint32_t parseIdOnly(const std::string & line);
    bool parse(const std::string & line);

    uint32_t getId() const {
        return id_;
    }

    void setId(uint32_t val) {
        id_ = val;
    }

    const std::string & getCName() const {
        return cname_;
    }

    void setCName(const std::string & val) {
        cname_ = val;
    }

    const std::string & getMslabel() const {
        return mslabel_;
    }

    void setMslabel(const std::string & val) {
        mslabel_ = val;
    }

    const std::string & getLabel() const {
        return label_;
    }

    void setLabel(const std::string & val) {
        label_ = val;
    }

    std::string toString() const;
private:
    uint32_t id_;
    std::string cname_;
    std::string mslabel_;
    std::string label_;
};
};