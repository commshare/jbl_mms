#pragma once
#include "../fmtp.h"
//@refer rfc3984.txt
namespace mms {
    struct H264Fmtp : public Fmtp {

        
        uint32_t profile_level_id;
        uint32_t max_mbps;
        uint32_t max_fs;
        uint32_t max_cpb;
        uint32_t max_dpb;
        uint32_t max_br;
        uint32_t redundant_pic_cap;
        uint32_t sprop_parameter_sets;
        uint32_t parameter_add;
        uint32_t packetization_mode;
        uint32_t sprop_interleaving_depth;
    };
};