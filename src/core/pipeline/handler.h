#pragma once
#include <memory>

namespace mms {
template<typename PKT>
class Handler {
public:
    Handler();

    virtual ~Handler() {

    }
public:
    bool processPkt(std::shared_ptr<PKT> pkt) = 0;
};

};