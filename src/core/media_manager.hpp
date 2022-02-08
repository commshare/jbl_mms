#pragma once
#include <boost/serialization/singleton.hpp> 
#include <unordered_map>
#include <mutex>

#include "core/media_source.hpp"

namespace mms {
class MediaManager : public boost::serialization::singleton<MediaManager> {
public:
    bool addSource(const std::string & source_name, std::shared_ptr<MediaSource> source) {
        std::lock_guard<std::mutex> lck(sources_mtx_);
        auto it = sources_.find(source_name);
        if (it != sources_.end()) {
            return false;
        }
        sources_[source_name] = source;
        return true;
    }

    std::shared_ptr<MediaSource> getSource(const std::string & source_name) {
        std::lock_guard<std::mutex> lck(sources_mtx_);
        auto it = sources_.find(source_name);
        if (it == sources_.end()) {
            return nullptr;
        }
        return it->second;
    }
private:
    std::mutex sources_mtx_;
    std::unordered_map<std::string, std::shared_ptr<MediaSource>> sources_;
};

};