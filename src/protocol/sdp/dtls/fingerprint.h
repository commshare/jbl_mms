#pragma once
#include <string>
namespace mms {
struct FingerPrint {
public:
    static std::string prefix;
    bool parse(const std::string & line);
    const std::string & getHashName() const {
        return hash_name;
    }

    void setHashName(const std::string & val) {
        hash_name = val;
    }

    const std::string & getHashVal() const {
        return hash_val;
    }

    void setHashVal(const std::string & val) {
        hash_val = val;
    }

    std::string toString() const;
private:
    std::string hash_name;
    std::string hash_val;
};
};