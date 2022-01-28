#pragma once

namespace mms {
template<typename T>
class Pipeline {
public:
    template<typename Q>
    void connectPipeLine(Q & q) {
        next_handler_ = q;
    }

    template<typename F, typename... ARGS>
    void connectPipeLine(F& t, ARGS... args) {
        next_handler_ = Pipeline<F>();
        connectPipeLine(args...);
    }
private:
    T next_handler_;
};

};