#pragma once
#include <vector>
#include "protocol/sdp/attribute/common/attribute.hpp"
namespace mms {
struct BundleAttr : public Attribute {
public:
    static std::string prefix;
    virtual bool parse(const std::string & line);

    const std::vector<std::string> & getMids() const {
        return mids;
    }

    void addMid(const std::string & mid) {
        mids.emplace_back(mid);
    }

    std::string toString() const;
private:
    std::vector<std::string> mids;
};
};