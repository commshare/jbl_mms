#pragma once
#include <memory>
#include "media_stream.hpp"
namespace mms {
template<typename PKT>
class MediaSink : public MediaStream<PKT> {
public:
    MediaSink() {

    }

    virtual ~MediaSink() {

    }

private:
    
};

};