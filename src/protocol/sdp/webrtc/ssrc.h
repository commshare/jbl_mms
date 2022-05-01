#pragma once
#include <string>
namespace mms {
struct Ssrc {
public:
    static std::string prefix;
    bool parse(const std::string & line);

    uint32_t getId() const {
        return id;
    }

    void setId(uint32_t val) {
        id = val;
    }

    const std::string & getCName() const {
        return cname;
    }

    void setCName(const std::string & val) {
        cname = val;
    }

    const std::string & getMslabel() const {
        return mslabel;
    }

    void setMslabel(const std::string & val) {
        mslabel = val;
    }

    const std::string & getLabel() const {
        return label;
    }

    void setLabel(const std::string & val) {
        label = val;
    }

    std::string toString() const;
private:
    uint32_t id;
    std::string cname;
    std::string mslabel;
    std::string label;
};
};