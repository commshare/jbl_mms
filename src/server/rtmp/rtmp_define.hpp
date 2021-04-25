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
#define RTMP_MESSAGE_USER_CONTROL       



#define RTMP_CHUNK_ID_PROTOCOL_CONTROL_MSG              0x02
#define RTMP_MSG_TYPE_ID_SET_CHUNK_SIZE                 0x01
#define RTMP_MSG_TYPE_ID_ABORT_MSG                      0x02
#define RTMP_MSG_TYPE_ID_ACKNOWLEDGEMENT                0x03
#define RTMP_MSG_TYPE_ID_USER_CONTROL_MSG               0x04
#define RTMP_MSG_TYPE_ID_WINDOW_ACKNOWLEDGEMENT_SIZE    0x05