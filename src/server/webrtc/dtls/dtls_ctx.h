#pragma once
namespace mms {
class DtlsCtx {
public:
    DtlsCtx() = default;
    ~DtlsCtx() {

    }
public:
    bool init();
};
};