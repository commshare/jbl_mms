#pragma once

#define RTMP_MESSAGE_AMF0_COMMAND       20
#define RTMP_MESSAGE_AMF3_COMMAND       17

#define RTMP_MESSAGE_AMF0_DATA          18
#define RTMP_MESSAGE_AMF3_DATA          15

#define RTMP_MESSAGE_AMF0_SHARED_OBJ    19
#define RTMP_MESSAGE_AMF3_SHARED_OBJ    16

#define RTMP_MESSAGE_VIDEO              9
#define RTMP_MESSAGE_AUDIO              8

#define RTMP_MESSAGE_AGGREGATE          22
#define RTMP_MESSAGE_USER_CONTROL       4

#define RTMP_MESSAGE_SET_CHUNK_SIZE                 0x01
#define RTMP_MESSAGE_ABORT_MSG                      0x02
#define RTMP_MESSAGE_ACKNOWLEDGEMENT                0x03
#define RTMP_MESSAGE_USER_CONTROL_MSG               0x04
#define RTMP_MESSAGE_WINDOW_ACKNOWLEDGEMENT_SIZE    0x05




#define RTMP_CHUNK_ID_PROTOCOL_CONTROL_MSG              0x02

#define RTMP_USER_EVENT_STREAM_BEGIN                    0x0000
#define RTMP_USER_EVENT_STREAM_EOF                      0x0001
#define RTMP_USER_EVENT_STREAM_DRY                      0x0002
#define RTMP_USER_EVENT_SET_BUFFER_LENGTH               0x0003
#define RTMP_USER_EVENT_STREAM_IS_RECORDED              0x0004
#define RTMP_USER_EVENT_PING_REQUEST                    0x0006
#define RTMP_USER_EVENT_PING_RESPONSE                   0x0007
