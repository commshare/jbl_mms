#pragma once
#include <vector>
#include <set>
#include <boost/circular_buffer.hpp>

#include "media_source.hpp"
#include "media_sink.hpp"

#include "server/rtmp/rtmp_protocol/rtmp_define.hpp"

#include "pipeline/pipeline.h"
#include "server/rtmp/handlers/rtmp_codec_parser.hpp"
#include "server/rtmp/handlers/rtmp_bandwidth_handler.hpp"
#include "server/rtmp/handlers/rtmp_stats_handler.hpp"
#include "server/rtmp/rtmp_message/data_message/rtmp_metadata_message.hpp"

#include "demuxer/rtmp_flv_demuxer.hpp"
#include "base/thread/thread_worker.hpp"
#include "base/sequence_pkt_buf.hpp"


namespace mms {
class RtmpMediaSource : public MediaSource {
public:
    RtmpMediaSource(ThreadWorker *worker) : MediaSource(worker), av_pkts_(2048), keyframe_indexes_(200) {

    }

    bool init() {
        return true;
    }

    virtual ~RtmpMediaSource() {

    }

    bool onAudioPacket(std::shared_ptr<RtmpMessage> audio_pkt) {
        // std::cout << " *********** onAudioPacket **************" << std::endl;
        latest_frame_index_ = av_pkts_.addPkt(audio_pkt);
        AudioTagHeader audio_tag_header;
        int32_t consumed = audio_tag_header.decode(audio_pkt->payload_, audio_pkt->payload_size_);
        if (consumed < 0) {
            return false;
        }

        if (audio_tag_header.isSeqHeader()) {
            audio_ready_ = true;
            audio_header_ = audio_pkt;
            std::cout << " *********** audio is ready **************" << std::endl;
        }

        if (!stream_ready_) {
            stream_ready_ = (has_audio_?audio_ready_:true) && (has_video_?video_ready_:true);
        }
        latest_audio_timestamp_ = audio_pkt->timestamp_;

        if (latest_frame_index_ >= 300 || latest_frame_index_%5 == 0) {
            std::lock_guard<std::mutex> lck(sinks_mtx_);
            for (auto & sink : sinks_) {
                sink->active();
            }
        }

        return true;
    }

    bool onVideoPacket(std::shared_ptr<RtmpMessage> video_pkt) {
        // std::cout << " *********** onVideoPacket **************" << std::endl;
        latest_frame_index_ = av_pkts_.addPkt(video_pkt);
        // 解析头部
        VideoTagHeader video_tag_header;
        int32_t consumed = video_tag_header.decode(video_pkt->payload_, video_pkt->payload_size_);
        if (consumed < 0) {
            return false;
        }

        if (video_tag_header.isKeyFrame() && !video_tag_header.isSeqHeader()) {// 关键帧索引
            // std::cout << "*************************** video is key *******************" << std::endl;
            keyframe_indexes_.push_back(latest_frame_index_);
        }  else if (video_tag_header.isSeqHeader()) {
            video_ready_ = true;
            // std::cout << "*************************** video is ready *******************" << std::endl;
            video_header_ = video_pkt;
        }

        if (!stream_ready_) {
            stream_ready_ = (has_audio_?audio_ready_:true) && (has_video_?video_ready_:true);
        }
        latest_video_timestamp_ = video_pkt->timestamp_;

        if (latest_frame_index_ >= 300 || latest_frame_index_%5 == 0) {
            std::lock_guard<std::mutex> lck(sinks_mtx_);
            for (auto & sink : sinks_) {
                sink->active();
            }
        }
        
        return true;
    }

    bool onMetadata(std::shared_ptr<RtmpMetaDataMessage> metadata_pkt) {
        std::cout << " *********** onMetadata **************" << std::endl;
        metadata_ = metadata_pkt;
        has_video_ = metadata_->hasVideo();
        has_audio_ = metadata_->hasAudio();
        return true;
    }

    bool addMediaSink(std::shared_ptr<MediaSink> media_sink) final {
        std::lock_guard<std::mutex> lck(sinks_mtx_);
        sinks_.insert(media_sink);
        media_sink->setSource(this);
        return true;
    }

    std::vector<std::shared_ptr<RtmpMessage>> getPkts(uint64_t &last_pkt_index, uint32_t max_count) {
        std::cout << "**************** last_pkt_index:" << last_pkt_index << " ,latest_frame_index_:" << latest_frame_index_ << " *******************" << std::endl;
        std::vector<std::shared_ptr<RtmpMessage>> pkts;
        if (last_pkt_index == -1) {
            pkts.emplace_back(metadata_->msg());
            if (has_video_) {
                pkts.emplace_back(video_header_);
            }
            
            if (has_audio_) {
                pkts.emplace_back(audio_header_);
            }

            if (has_video_) {
                auto it = keyframe_indexes_.rbegin();
                while(it != keyframe_indexes_.rend()) {
                    auto t = av_pkts_.getPkt(*it);
                    if (t) {
                        if (latest_video_timestamp_ - t->timestamp_ >= 2000) {
                            break;
                        }
                        it++;
                    } else {
                        break;
                    }
                }

                uint64_t start_idx = *it;
                uint32_t pkt_count = 0;
                while(start_idx <= latest_frame_index_ && pkt_count < max_count) {
                    auto t = av_pkts_.getPkt(start_idx);
                    if (t) {
                        pkts.emplace_back(av_pkts_.getPkt(start_idx));
                        pkt_count++;
                    }
                    
                    start_idx++;
                }
                last_pkt_index = start_idx;
            }
        } else {
            uint64_t start_idx = last_pkt_index;
            uint32_t pkt_count = 0;
            while(start_idx <= latest_frame_index_ && pkt_count < max_count) {
                auto t = av_pkts_.getPkt(start_idx);
                if (t) {
                    pkts.emplace_back(av_pkts_.getPkt(start_idx));
                    pkt_count++;
                }
                
                start_idx++;
            }
            last_pkt_index = start_idx;
        }

        return pkts;
    }

    void close() {
        std::lock_guard<std::mutex> lck(sinks_mtx_);
        for (auto & sink : sinks_) {
            sink->close();
        }
        sinks_.clear();
    }
protected:
    RtmpFlvDemuxer rtmp_flv_demuxer_;
    SequencePktBuf<RtmpMessage> av_pkts_;
    Pipeline<
        RtmpCodecParser,
        RtmpBandwidthHandler,
        RtmpStatsHandler
    > pipeline_;

    std::shared_ptr<RtmpMetaDataMessage> metadata_;
    std::shared_ptr<RtmpMessage> video_header_;
    std::shared_ptr<RtmpMessage> audio_header_;
    boost::circular_buffer<uint64_t> keyframe_indexes_;
    uint64_t latest_frame_index_ = 0;
protected:
    bool has_video_; 
    bool video_ready_ = false;
    bool has_audio_;
    bool audio_ready_ = false;
    int32_t latest_video_timestamp_ = 0;
    int32_t latest_audio_timestamp_ = 0;
};
};