#pragma once
#include <memory>

#include "stun_define.hpp"
namespace mms {
struct StunBindingErrorResponseMsg : public StunMsg {
public:
    StunBindingErrorResponseMsg(int32_t code) {
        header.type = STUN_BINDING_ERROR_RESPONSE;
        auto attr = std::make_unique<StunErrorCodeAttr>(code, "");
        attrs.emplace_back(std::move(attr));
    }
};
};