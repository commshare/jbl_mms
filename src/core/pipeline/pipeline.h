#pragma once

namespace mms {
template<typename... ARGS> 
class Pipeline;

template<typename T, typename... ARGS>
class Pipeline<T, ARGS...> : public Pipeline<ARGS...> {
public:
    Pipeline() {
    }

    template<typename M>
    inline bool processPkt(std::shared_ptr<M> msg) {
        if (!handler_.processPkt(msg)) {
            return false;
        }
        
        return Pipeline<ARGS...>::processPkt(msg);
    }

    T handler_;
};

template<typename T> 
class Pipeline<T> {
public:
    Pipeline() {
    }

    template<typename M>
    inline bool processPkt(std::shared_ptr<M> msg) {
        if (!handler_.processPkt(msg)) {
            return false;
        }
        
        return true;
    }

    T handler_;
};

};