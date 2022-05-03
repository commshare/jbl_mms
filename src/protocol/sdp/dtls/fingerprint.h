#pragma once
#include <string>
namespace mms {
struct FingerPrint {
public:
    static std::string prefix;
    bool parse(const std::string & line);
    FingerPrint() = default;
    FingerPrint(const std::string & hash_name, const std::string & hash_val) : hash_name_(hash_name), hash_val_(hash_val) {

    }

    const std::string & getHashName() const {
        return hash_name_;
    }

    void setHashName(const std::string & val) {
        hash_name_ = val;
    }

    const std::string & getHashVal() const {
        return hash_val_;
    }

    void setHashVal(const std::string & val) {
        hash_val_ = val;
    }

    std::string toString() const;
private:
    std::string hash_name_;
    std::string hash_val_;
};
};