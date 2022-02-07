#include "rtmp_metadata_message.hpp"
using namespace mms;
RtmpMetaDataMessage::RtmpMetaDataMessage() {

}

RtmpMetaDataMessage::~RtmpMetaDataMessage() {

}

int32_t RtmpMetaDataMessage::decode(std::shared_ptr<RtmpMessage> rtmp_msg) {
    int32_t consumed = 0;
    int32_t pos = 0;
    const uint8_t *payload = rtmp_msg->payload_;
    int32_t len = rtmp_msg->payload_size_;
    Amf0String name;
    Amf0EcmaArray metadata;
    consumed = name.decode(payload, len);
    if (consumed < 0) {
        return -1;
    }
    pos += consumed;
    payload += consumed;
    len -= consumed;
    if (name.getValue() == "@setDataFrame") {
        consumed = name.decode(payload, len);
        if (consumed < 0) {
            return -2;
        }
        pos += consumed;
        payload += consumed;
        len -= consumed;
    }

    consumed = metadata.decode(payload, len);
    if (consumed < 0) {
        return -3;
    }
    pos += consumed;
    payload += consumed;
    len -= consumed;
    
    cache_msg_ = rtmp_msg;
    {// get metadata info
        auto t = metadata.getProperty<Amf0Number>("audiocodecid");
        if (t) {
            audio_codec_id_ = (AudioTagHeader::SoundFormat)*t;
            has_audio_ = true;
        }

        t = metadata.getProperty<Amf0Number>("audiochannels");
        if (t) {
            audio_channels_ = *t;
        }

        t = metadata.getProperty<Amf0Number>("audiodatarate");
        if (t) {
            audio_datarate_ = *t;
        }

        t = metadata.getProperty<Amf0Number>("audiosamplerate");
        if (t) {
            audio_sample_rate_ = *t;
        }

        t = metadata.getProperty<Amf0Number>("audiosamplesize");
        if (t) {
            audio_sample_size_ = *t;
        }

        t = metadata.getProperty<Amf0Number>("duration");
        if (t) {
            duration_ = *t;
        }

        auto m = metadata.getProperty<Amf0String>("encoder");
        if (m) {
            encoder_ = *m;
        }

        t = metadata.getProperty<Amf0Number>("fileSize");
        if (t) {
            file_size_ = *t;
        }

        t = metadata.getProperty<Amf0Number>("framerate");
        if (t) {
            frame_rate_ = *t;
        }

        t = metadata.getProperty<Amf0Number>("height");
        if (t) {
            height_ = *t;
        }

        t = metadata.getProperty<Amf0Number>("width");
        if (t) {
            width_ = *t;
        }
        
        t = metadata.getProperty<Amf0Boolean>("stereo");
        if (t) {
            stereo_ = *t;
        }

        t = metadata.getProperty<Amf0Boolean>("videocodecid");
        if (t) {
            video_codec_id_ = (VideoTagHeader::CodecID)*t;
            has_video_ = true;
        }

        t = metadata.getProperty<Amf0Boolean>("videodatarate");
        if (t) {
            video_data_rate_ = *t;
        }        
    }
    return pos;
}

std::shared_ptr<RtmpMessage> RtmpMetaDataMessage::encode() {
    //todo implement this method
    return nullptr;
}
