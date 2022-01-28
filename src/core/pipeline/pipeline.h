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
    bool processPkt(std::shared_ptr<M> msg) {
        if (!cur_handler_.processPkt(msg)) {
            return false;
        }
        
        if (!Pipeline<ARGS...>::processPkt(msg)) {
            return false;
        }
        return true;
    }

    T cur_handler_;
};

template<typename T> 
class Pipeline<T> {
public:
    Pipeline() {
    }

    template<typename M>
    inline bool processPkt(std::shared_ptr<M> msg) {
        if (!cur_handler_.processPkt(msg)) {
            return false;
        }
        
        return true;
    }

    T cur_handler_;
};

};