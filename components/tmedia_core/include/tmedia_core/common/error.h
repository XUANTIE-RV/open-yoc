/*
 * Copyright (C) 2021-2022 Alibaba Group Holding Limited
 */

#ifndef TM_ERROR_H
#define TM_ERROR_H

#include <errno.h>
#include <string>
#ifdef __linux__
#include <tmedia_config.h>
#endif

using namespace std;

#define MKBYTETAG(a,b,c,d) ((a) | ((b) << 8) | ((c) << 16) | ((unsigned)(d) << 24))
#define MAKE_FOURCC(a, b, c, d) (-(int)MKBYTETAG(a, b, c, d))
#define MKTAG_NEGA(e) (-(e))

class TMResult
{
public:
    typedef struct {
        int ID;
        const char *String;
    } ErrorEntry_s;

    enum ID
    {
        TM_OK = 0,                                  // OK
        TM_UNKNOWN    = MKTAG_NEGA(0x80000000),     // Unkonw Error

        /* POSIX standard error with negation (partly abridged), order by errno */
        TM_EPERM      = MKTAG_NEGA(EPERM),          // Operation not permitted
        TM_ENOENT     = MKTAG_NEGA(ENOENT),         // No such file or directory
        TM_EIO        = MKTAG_NEGA(EIO),            // I/O error
        TM_ENXIO      = MKTAG_NEGA(ENXIO),          // No such device or address
        TM_E2BIG      = MKTAG_NEGA(E2BIG),          // Argument list too long
        TM_EBADF      = MKTAG_NEGA(EBADF),          // Bad file number
        TM_EAGAIN     = MKTAG_NEGA(EAGAIN),         // Try again
        TM_ENOMEM     = MKTAG_NEGA(ENOMEM),         // Out of memory
        TM_EACCES     = MKTAG_NEGA(EACCES),         // Permission denied
        TM_EFAULT     = MKTAG_NEGA(EFAULT),         // Bad address
        TM_EBUSY      = MKTAG_NEGA(EBUSY),          // Device or resource busy
        TM_ENODEV     = MKTAG_NEGA(ENODEV),         // No such device
        TM_EINVAL     = MKTAG_NEGA(EINVAL),         // Invalid argument
        TM_EFBIG      = MKTAG_NEGA(EFBIG),          // File too large

        /* TMedia extend error numbers for specific error order by FCC */
        TM_BACKEND_ERROR      = MAKE_FOURCC('B','K','N','D'),   // Backend error
        TM_BUF_TOO_SMALL      = MAKE_FOURCC('B','U','F','S'),   // Buffer too small
        TM_CAMERA_NOT_FOUND   = MAKE_FOURCC('C','A','M','N'),   // Camera not found
        TM_DECODER_NOT_FOUND  = MAKE_FOURCC('D','E','C','N'),   // Decoder not found
        TM_DEMUXER_NOT_FOUND  = MAKE_FOURCC('D','E','M','N'),   // Demuxer not found
        TM_ENCODER_NOT_FOUND  = MAKE_FOURCC('E','N','C','N'),   // Encoder not found
        TM_ENTITY_NOT_FOUND   = MAKE_FOURCC('E','N','T','N'),   // Entity not found
        TM_EVENT_PUSH_FAILED  = MAKE_FOURCC('E','V','P','F'),   // Event push failed
        TM_EOF                = MAKE_FOURCC('E','O','F',' '),   // End of file or stream
        TM_FORMAT_INVALID     = MAKE_FOURCC('F','M','A','T'),   // Format Error
        TM_MAPVAL_FAIL        = MAKE_FOURCC('M','A','P','V'),   // Map backend values failed
        TM_MUXER_NOT_FOUND    = MAKE_FOURCC('M','U','X','N'),   // Muxer not found
        TM_NOT_IMPLEMENTED    = MAKE_FOURCC('N','I','M','P'),   // Operate or feature not implemented
        TM_NOT_SUPPORT        = MAKE_FOURCC('N','S','P','T'),   // Operate or feature not supported
        TM_PEER_PAD_NOT_FOUND = MAKE_FOURCC('P','E','P','N'),   // Peer Pad not found
        TM_PIPE_OPERATE_FAILED= MAKE_FOURCC('P','I','O','F'),   // Pipe operate failed
        TM_PROPERTY_NOT_FOUND = MAKE_FOURCC('P','R','O','P'),   // Property not found or support
        TM_PROTOCOL_NOT_FOUND = MAKE_FOURCC('P','R','O','T'),   // Protocol not found or support
        TM_STATE_ERROR        = MAKE_FOURCC('S','T','A','T'),   // State error
        TM_TIMEOUT            = MAKE_FOURCC('T','O','U','T'),   // time out
    };

    static const char* StrError(TMResult::ID result);
    static const char* StrError(int result);
};

#endif  /* TM_ERROR_H */
