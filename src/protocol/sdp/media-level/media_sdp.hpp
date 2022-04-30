#pragma once
#include <string>
#include <optional>
#include <vector>

#include "protocol/sdp/ice/ice_ufrag.h"
#include "protocol/sdp/ice/ice_pwd.h"
#include "protocol/sdp/ice/ice_options.h"
#include "protocol/sdp/webrtc/extmap.hpp"
#include "protocol/sdp/session-level/connection_info.hpp"
#include "protocol/sdp/media-level/mid.h"

#include "protocol/sdp/attribute/common/recvonly.hpp"
#include "protocol/sdp/attribute/common/sendonly.hpp"
#include "protocol/sdp/attribute/common/sendrecv.hpp"
#include "protocol/sdp/attribute/common/rtpmap.h"
#include "protocol/sdp/attribute/common/maxptime.hpp"
// Media description, if present
//     m=  (media name and transport address)
//     i=* (media title)
//     c=* (connection information -- optional if included at
//         session level)
//     b=* (zero or more bandwidth information lines)
//     k=* (encryption key)
//     a=* (zero or more media attribute lines)
// Some attributes (the ones listed in Section 6 of this memo)
//    have a defined meaning, but others may be added on an application-,
//    media-, or session-specific basis.  An SDP parser MUST ignore any
//    attribute it doesn't understand.

// m=audio 9 UDP/TLS/RTP/SAVPF 111 63 103 104 9 0 8 106 105 13 110 112 113 126
// c=IN IP4 0.0.0.0
// a=rtcp:9 IN IP4 0.0.0.0
// a=ice-ufrag:zV6U
// a=ice-pwd:aM/wRMqaSFysiElHwmg+xFEO
// a=ice-options:trickle
// a=fingerprint:sha-256 1E:4C:4A:70:97:1A:23:6D:CB:96:D4:25:A4:24:35:16:F3:2D:D1:10:40:7E:04:B3:47:23:B5:8D:0D:AC:8D:50
// a=setup:actpass
// a=mid:0
// a=extmap:1 urn:ietf:params:rtp-hdrext:ssrc-audio-level
// a=extmap:2 http://www.webrtc.org/experiments/rtp-hdrext/abs-send-time
// a=extmap:3 http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01
// a=extmap:4 urn:ietf:params:rtp-hdrext:sdes:mid
// a=sendrecv
// a=msid:Yx5UaozW22H01RBGCVncj2CErPQ2vaZKuGOf 6830a387-98d3-483b-8c59-8ddc1ea05860
// a=rtcp-mux
// a=rtpmap:111 opus/48000/2
// a=rtcp-fb:111 transport-cc
// a=fmtp:111 minptime=10;useinbandfec=1
// a=rtpmap:63 red/48000/2
// a=fmtp:63 111/111
// a=rtpmap:103 ISAC/16000
// a=rtpmap:104 ISAC/32000
// a=rtpmap:9 G722/8000
// a=rtpmap:0 PCMU/8000
// a=rtpmap:8 PCMA/8000
// a=rtpmap:106 CN/32000
// a=rtpmap:105 CN/16000
// a=rtpmap:13 CN/8000
// a=rtpmap:110 telephone-event/48000
// a=rtpmap:112 telephone-event/32000
// a=rtpmap:113 telephone-event/16000
// a=rtpmap:126 telephone-event/8000
// a=ssrc:3171581228 cname:gjOR7q3vsC3BwZ/c
// a=ssrc:3171581228 msid:Yx5UaozW22H01RBGCVncj2CErPQ2vaZKuGOf 6830a387-98d3-483b-8c59-8ddc1ea05860
// a=ssrc:3171581228 mslabel:Yx5UaozW22H01RBGCVncj2CErPQ2vaZKuGOf
// a=ssrc:3171581228 label:6830a387-98d3-483b-8c59-8ddc1ea05860

namespace mms
{
    struct MediaSdp
    {
    public:
        enum Direction
        {
            sendonly = 0,
            sendrecv = 1,
            recvonly = 2,
        };
        static std::string prefix;
        virtual bool parse(const std::string &line);
        bool parseAttr(const std::string &line);

        const std::string &getMedia() const
        {
            return media; // audio text video ...
        }

        void setMedia(const std::string &val)
        {
            media = val;
        }

        const std::vector<uint16_t> &getPorts() const
        {
            return ports;
        }

        void addPort(uint16_t val)
        {
            ports.push_back(val);
        }

        void setPorts(const std::vector<uint16_t> &val)
        {
            ports = val;
        }

        const std::string &getProto() const
        {
            return proto;
        }

        void setProto(const std::string &val)
        {
            proto = val;
        }

        const std::vector<uint16_t> &getFmts() const
        {
            return fmts;
        }

        void addFmt(uint16_t val)
        {
            fmts.push_back(val);
        }

        void setFmts(const std::vector<uint16_t> &val)
        {
            fmts = val;
        }

        const std::optional<IceUfrag> &getIceUfrag() const
        {
            return ice_ufrag;
        }

        void setIceUfrag(const IceUfrag &val)
        {
            ice_ufrag = val;
        }

        const std::optional<IcePwd> &getIcePwd() const
        {
            return ice_pwd;
        }

        void setIcePwd(const IcePwd &val)
        {
            ice_pwd = val;
        }

        const std::optional<IceOption> getIceOption() const
        {
            return ice_option;
        }

        void setIceOption(const IceOption &val)
        {
            ice_option = val;
        }

        const std::vector<Extmap> &getExtmap() const
        {
            return ext_maps;
        }

        void setExtmap(const std::vector<Extmap> &val)
        {
            ext_maps = val;
        }

        void addExtmap(const Extmap &val)
        {
            ext_maps.push_back(val);
        }

        Direction getDir() const
        {
            return dir;
        }

        void setDir(Direction val)
        {
            dir = val;
        }

    private:
        // <media> is the media type.  Currently defined media are "audio",
        //   "video", "text", "application", and "message", although this list
        //   may be extended in the future (see Section 8).
        std::string media;
        // <port> is the transport port to which the media stream is sent.  The
        //       meaning of the transport port depends on the network being used as
        //       specified in the relevant "c=" field, and on the transport
        //       protocol defined in the <proto> sub-field of the media field.
        //       Other ports used by the media application (such as the RTP Control
        //       Protocol (RTCP) port [19]) MAY be derived algorithmically from the
        //       base media port or MAY be specified in a separate attribute (for
        //       example, "a=rtcp:" as defined in [22]).

        //       If non-contiguous ports are used or if they don't follow the
        //       parity rule of even RTP ports and odd RTCP ports, the "a=rtcp:"
        //       attribute MUST be used. （如果不遵从偶数RTP端口和奇数RTCP端口的规则，那么要用a=rtcp:指定RTCP端口）
        //       Applications that are requested to send
        //       media to a <port> that is odd and where the "a=rtcp:" is present
        //       MUST NOT subtract 1 from the RTP port: that is, they MUST send the
        //       RTP to the port indicated in <port> and send the RTCP to the port
        //       indicated in the "a=rtcp" attribute.
        //       以上也就是说，如果没有a=rtcp那么就使用奇偶规则定义rtcp端口

        //       For applications where hierarchically encoded streams are being
        //       sent to a unicast address, it may be necessary to specify multiple
        //       transport ports.  This is done using a similar notation to that
        //       used for IP multicast addresses in the "c=" field:
        //          m=<media> <port>/<number of ports> <proto> <fmt> ...

        //       In such a case, the ports used depend on the transport protocol.
        //       For RTP, the default is that only the even-numbered ports are used
        //       for data with the corresponding one-higher odd ports used for the
        //       RTCP belonging to the RTP session, and the <number of ports>
        //       denoting the number of RTP sessions.  For example:

        //          m=video 49170/2 RTP/AVP 31

        //       would specify that ports 49170 and 49171 form one RTP/RTCP pair
        //       and 49172 and 49173 form the second RTP/RTCP pair.  RTP/AVP is the
        //       transport protocol and 31 is the format (see below).  If non-
        //       contiguous ports are required, they must be signalled using a
        //       separate attribute (for example, "a=rtcp:" as defined in [22]).

        //       If multiple addresses are specified in the "c=" field and multiple
        //       ports are specified in the "m=" field, a one-to-one mapping from
        //       port to the corresponding address is implied.  For example:

        //          c=IN IP4 224.2.1.1/127/2
        //          m=video 49170/2 RTP/AVP 31

        //       would imply that address 224.2.1.1 is used with ports 49170 and
        //       49171, and address 224.2.1.2 is used with ports 49172 and 49173.

        //       The semantics of multiple "m=" lines using the same transport
        //       address are undefined.  This implies that, unlike limited past
        //       practice, there is no implicit grouping defined by such means and
        //       an explicit grouping framework (for example, [18]) should instead
        //       be used to express the intended semantics.

        //    <proto> is the transport protocol.  The meaning of the transport
        //       protocol is dependent on the address type field in the relevant
        //       "c=" field.  Thus a "c=" field of IP4 indicates that the transport
        //       protocol runs over IP4.  The following transport protocols are
        //       defined, but may be extended through registration of new protocols
        //       with IANA (see Section 8):

        //       *  udp: denotes an unspecified protocol running over UDP.

        //       *  RTP/AVP: denotes RTP [19] used under the RTP Profile for Audio
        //          and Video Conferences with Minimal Control [20] running over
        //          UDP.

        //       *  RTP/SAVP: denotes the Secure Real-time Transport Protocol [23]
        //          running over UDP.
        // The main reason to specify the transport protocol in addition to
        //   the media format is that the same standard media formats may be
        //   carried over different transport protocols even when the network
        //   protocol is the same -- a historical example is vat Pulse Code
        //   Modulation (PCM) audio and RTP PCM audio; another might be TCP/RTP
        //   PCM audio.  In addition, relays and monitoring tools that are
        //   transport-protocol-specific but format-independent are possible.
        std::vector<uint16_t> ports;
        std::string proto;
        // <fmt> is a media format description.  The fourth and any subsequent
        //   sub-fields describe the format of the media.  The interpretation
        //   of the media format depends on the value of the <proto> sub-field.

        //   If the <proto> sub-field is "RTP/AVP" or "RTP/SAVP" the <fmt>
        //   sub-fields contain RTP payload type numbers.  When a list of
        //   payload type numbers is given, this implies that all of these
        //   payload formats MAY be used in the session, but the first of these
        //   formats SHOULD be used as the default format for the session.  For
        //   dynamic payload type assignments the "a=rtpmap:" attribute (see
        //   Section 6) SHOULD be used to map from an RTP payload type number
        //   to a media encoding name that identifies the payload format.  The
        //   "a=fmtp:"  attribute MAY be used to specify format parameters (see
        //   Section 6).
        std::vector<uint16_t> fmts;

        std::optional<IceUfrag> ice_ufrag;
        std::optional<IcePwd> ice_pwd;
        std::optional<IceOption> ice_option;
        Direction dir;
        std::vector<Rtpmap> rtpmaps;
        std::optional<MaxPTimeAttr> max_ptime;
        std::vector<Extmap> ext_maps;

        std::optional<ConnectionInfo> connection_info;
        std::optional<MidAttr> mid;
        std::vector<std::string> bandwidth_information;
        std::optional<std::string> encryption_key;
    };
};