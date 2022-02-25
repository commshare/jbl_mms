#pragma once
#include <string_view>
// 5.14.  Media Descriptions ("m=")
//       m=<media> <port> <proto> <fmt> ...
// A session description may contain a number of media descriptions.
//    Each media description starts with an "m=" field and is terminated by
//    either the next "m=" field or by the end of the session description.
//    A media field has several sub-fields:
namespace mms {
struct MediaDescription {
public:
    std::string_view raw_string;
    // <media> is the media type.  Currently defined media are "audio",
    //   "video", "text", "application", and "message", although this list
    //   may be extended in the future (see Section 8).
    std::string_view media;
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
    std::string_view port;
    std::string_view proto;
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
    std::string_view fmt;
};
};