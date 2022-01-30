/*
MIT License

Copyright (c) 2021 jiangbaolin

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#pragma once
#include <arpa/inet.h>
#include <string.h>
#include <vector>
#include <memory>

namespace mms {
#pragma packed(1)
#define FLV_IDENTIFIER  "FLV"
struct FlvHeader {
    uint8_t signature[3];
    uint8_t version = 0x01;
    using Flags = struct {
        uint8_t reserved1:5;
        uint8_t audio:1;
        uint8_t reserved2:1;
        uint8_t video:1;
    };
    Flags flag;
    uint32_t data_offset = 0x09;
};

struct FlvTagHeader {
    enum TagType : uint8_t {
        AudioTag    = 8,
        VideoTag    = 9,
        ScriptTag   = 18,
    };

    struct Type {
        uint8_t reserved:2;
        uint8_t filter:1;
        TagType type:5;
    };

    Type tag_type;
    uint8_t data_size[3];
    uint8_t timestamp[3];
    uint8_t timestamp_ext;
    uint8_t stream_id[3];
};

struct AudioTagHeader {
    enum SoundFormat : uint8_t {
        LinearPCM_PE    = 0,    //0 = Linear PCM, platform endian
        ADPCM           = 1,    //1 = ADPCM
        MP3             = 2,    //2 = MP3
        LinearPCM_LE    = 3,    //3 = Linear PCM, little endian
        Nellymoser16kHz = 4,    //4 = Nellymoser 16 kHz mono
        Nellymoser8kHz  = 5,    //5 = Nellymoser 8 kHz mono
        Nellymoser      = 6,    //6 = Nellymoser
        G711ALawPCM     = 7,    //7 = G.711 A-law logarithmic PCM
        G711MuLawPCM    = 8,    //8 = G.711 mu-law logarithmic PCM
        Reserved        = 9,    //9 = reserved
        AAC             = 10,   //10 = AAC
        Speex           = 11,   //11 = Speex
        MP38kHZ         = 12,   //14 = MP3 8 kHz
        DeviceSpecific  = 13,   //15 = Device-specific sound
    };

    enum SoundRate : uint8_t {
        KHZ_5P5         = 0,
        KHZ_11          = 1,
        KHZ_22          = 2,
        KHZ_44          = 3,
    };

    enum SoundSize : uint8_t {
        Sample_8bit      = 0,
        Sample_16bit     = 1,
    };

    enum SoundType : uint8_t {
        MonoSound        = 0,
        StereoSound      = 1,
    };

    struct SoundInfo {
        SoundFormat sound_format:4;
        SoundRate sound_rate:2;
        SoundSize sound_size:1;
        SoundType sound_type:1;
    };

    SoundInfo sound_info;
    enum AACPacketType : uint8_t {
        AACSequenceHeader = 0,
        AACRaw            = 1,  
    };

    AACPacketType aac_packet_type;//IF SoundFormat == 10

    int32_t decode(const uint8_t *data, size_t len) {
        const uint8_t *buf = data;
        if (len < 1) {
            return -1;
        }
        sound_info = (SoundInfo)(*data);
        len--;
        data++;

        if (sound_info.sound_format == AAC) {
            if (len < 1) {
                return -2;
            }
            aac_packet_type = (AACPacketType)(*data); 
            len--;
            data++;
        }
        return data - buf;
    }
};

// 我们不需要解析音频编码细节，直接存储就行了
struct AudioSpecificConfig {//AudioSpecificConfig iso-14496-3 @1.6.2
    uint8_t *payload;
    size_t  payload_size;
    int32_t decode(const uint8_t *data, size_t len) {
        payload = (uint8_t*)data;
        payload_size = len;
        return len;
    }
};

struct AACRawFrame {
    uint8_t *payload;
    size_t payload_size;
    int32_t decode(const uint8_t *data, size_t len) {
        payload = (uint8_t*)data;
        payload_size = len;
        return len;
    }
};

struct AACAudioData {
    union {
        AudioSpecificConfig audio_specific_config;
        AACRawFrame         aac_raw_frame;
    } u;
};

struct OtherAudioData {
    uint8_t data[1];
    size_t  payload_size;
}

struct AudioTagBody {
    union {
        AACAudioData aac_audio_data;
        OtherAudioData other_audio_data;
    } u;
};

struct FlvAudioTag {
    AudioTagHeader audio_tag_header;
    AudioTagBody   audio_tag_body;
    int32_t decode(const uint8_t *buf, size_t len) {
        const uint8_t *buf_start = buf;
        int32_t consumed = audio_tag_header.decode(buf, len);
        if (consumed < 0) {
            return -1;
        }
        buf += consumed;
        len -= consumed;

        if (audio_tag_header.sound_info.sound_format == AudioTagHeader::AAC) {
            if (audio_tag_header.aac_packet_type == 0) {
                consumed = audio_tag_body.u.aac_audio_data.u.audio_specific_config.decode(buf, len);
            } else if (audio_tag_header.aac_packet_type == 1){
                consumed = audio_tag_body.u.aac_audio_data.u.aac_raw_frame.decode(buf, len);
            } else {//这种情况不可能出现，报错
                return -2;
            }
            buf += consumed;
            len -= consumed;
        }

        consumed = audio_tag_body.decode(buf, len);
        if (consumed < 0) {
            return -2;
        }
        buf += consumed;
        len -= consumed;
        return buf - buf_start;
    }
};

struct VideoTagHeader {
    enum FrameType : uint8_t {
        KeyFrame            = 1,
        InterFrame          = 2,
        DisposableFrame     = 3,
        GereratedKeyFrame   = 4,
        VideoInfoFrame      = 5,
    };

    enum CodecID : uint8_t {
        SorensonH264        = 2,
        ScreenVideo         = 3,
        On2VP6              = 4,
        On2VP6_2            = 5,
        ScreenVideo_2       = 6,
        AVC                 = 7,
    };
    
    enum AVCPacketType : uint8_t {
        AVCSequenceHeader   = 0,
        AVCNALU             = 1,
        AVCEofSequence      = 2,
    };

    FrameType       frame_type:4;
    CodecID         codec_id:4;
    AVCPacketType   avc_packet_type;
    uint32_t        composition_time;

    int32_t decode(const uint8_t *data, size_t len) {
        auto buf_start = data;
        if (len < 1) {
            return -1;
        }

        frame_type = (FrameType)((*data)&0x0f);
        codec_id = CodecID(((*data)>>4)&0x0f);
        len--;
        data++;

        if (codec_id == AVC) {
            if (len < 4) {
                return -2;
            }
            avc_packet_type = (AVCPacketType)(*data);
            data++;
            len--;
            composition_time = 0;
            uint8_t *p = (uint8_t*)&composition_time;
            p[0] = data[0];
            p[1] = data[1];
            p[2] = data[2];
            composition_time = ntohl(composition_time);
            data += 3;
            len -= 3;
        }        
        return data - buf_start;
    }
};

struct AVCDecoderConfigurationRecord {
    uint8_t *payload;
    size_t payload_size;
    int32_t decode(const uint8_t *data, size_t len) {
        payload = (uint8_t*)data;
        payload_size = len;
        return len;
    }
};

struct AVCRawFrame {
    uint8_t *payload;
    size_t payload_size;
    int32_t decode(const uint8_t *data, size_t len) {
        payload = (uint8_t*)data;
        payload_size = len;
        return len;
    }
};

struct AVCVideoPacket {
    union {
        AVCDecoderConfigurationRecord avc_decoder_config_record;
        AVCRawFrame                   avc_raw_frame;
    }u;
};

struct OtherVideoPacket {
    uint8_t *payload;
    size_t payload_size;
    int32_t decode(const uint8_t *data, size_t len) {
        payload = (uint8_t*)data;
        payload_size = len;
        return len;
    }
};

struct VideoTagBody {
    union {
        AVCVideoPacket avc_video_packet;
        OtherVideoPacket other_video_packet;
    }u;
};

struct FlvVideoTag {
    VideoTagHeader video_tag_header;
    VideoTagBody video_tag_body;

    int32_t decode(const uint8_t *data, size_t len) {
        auto buf_start = data;
        auto consumed = video_tag_header.decode(data, len);
        if (consumed < 0) {
            return -1;
        }
        data += consumed;
        len -= consumed;
        if (video_tag_header.codec_id == AVC) {
            if (video_tag_header.avc_packet_type == AVCSequenceHeader) {
                consumed = video_tag_body.u.avc_video_packet.u.avc_decoder_config_record.decode(data, len);
            } else {
                consumed = video_tag_body.u.avc_video_packet.u.avc_raw_frame.decode(data, len);
            }

            if (consumed < 0) {
                return -2;
            }

            data += consumed;
            len -= consumed;
        }
        return data - buf_start;
    }
};

template<typename T>
struct FlvTag {
    FlvTagHeader tag_header;
    T            tag_data;
};

struct FlvBody {
    uint32_t prev_tag_size;
};

};