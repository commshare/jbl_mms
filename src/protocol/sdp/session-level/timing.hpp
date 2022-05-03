#pragma once
#include <string>
namespace mms {
struct Timing {
public:
    static std::string prefix;
    Timing() = default;
    Timing(uint64_t start_time, uint64_t stop_time) {
        start_time_ = start_time;
        stop_time_ = stop_time;
    }

    bool parse(const std::string & line);

    uint64_t getStartTime() const {
        return start_time_;
    }

    void setStartTime(uint64_t val) {
        start_time_ = val;
    }

    uint64_t getStopTime() const {
        return stop_time_;
    }

    void setStopTime(uint64_t val) {
        stop_time_ = val;
    }

    std::string toString() const;
private:
    uint64_t start_time_ = 0;
    uint64_t stop_time_ = 0;
};
};