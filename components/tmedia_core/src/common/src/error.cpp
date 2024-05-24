/*
 * Copyright (C) 2021-2022 Alibaba Group Holding Limited
 */

#include <tmedia_core/common/error.h>
using namespace std;

const char* TMResult::StrError(TMResult::ID result)
{
    switch (result)
    {
    case TMResult::TM_OK:
        return "OK";
    case TMResult::TM_UNKNOWN:
        return "Unknown error occurred";

    /* POSIX standard error with negation (partly abridged), order by errno */
    case TMResult::TM_EPERM:
        return "Operation not permitted";
    case TMResult::TM_ENOENT:
        return "No such file or directory";
    case TMResult::TM_EIO:
        return "I/O error";
    case TMResult::TM_ENXIO:
        return "No such device or address";
    case TMResult::TM_E2BIG:
        return "Argument list too long";
    case TMResult::TM_EBADF:
        return "Bad file number";
    case TMResult::TM_EAGAIN:
        return "Try again";
    case TMResult::TM_ENOMEM:
        return "Out of memory";
    case TMResult::TM_EACCES:
        return "Permission denied";
    case TMResult::TM_EFAULT:
        return "Bad address";
    case TMResult::TM_EBUSY:
        return "Device or resource busy";
    case TMResult::TM_ENODEV:
        return "No such device";
    case TMResult::TM_EINVAL:
        return "Invalid argument";
    case TMResult::TM_EFBIG:
        return "File too large";

    /* TMedia extend error numbers for specific error order by FCC */
    case TMResult::TM_BUF_TOO_SMALL:
        return "Buffer too small";
    case TMResult::TM_CAMERA_NOT_FOUND:
        return "Camera not found";
    case TMResult::TM_DECODER_NOT_FOUND:
        return "Decoder not found";
    case TMResult::TM_DEMUXER_NOT_FOUND:
        return "Demuxer not found";
    case TMResult::TM_ENCODER_NOT_FOUND:
        return "Encoder not found";
    case TMResult::TM_ENTITY_NOT_FOUND:
        return "Entity not found";
    case TMResult::TM_EVENT_PUSH_FAILED:
        return "Event push failed";
    case TMResult::TM_EOF:
        return "End of file or stream";
    case TMResult::TM_FORMAT_INVALID:
        return "Format Error";
    case TMResult::TM_MAPVAL_FAIL:
        return "Map backend values failed";
    case TMResult::TM_MUXER_NOT_FOUND:
        return "Muxer not found";
    case TMResult::TM_NOT_IMPLEMENTED:
        return "Operate or feature not implemented";
    case TMResult::TM_NOT_SUPPORT:
        return "Operation or feature not supported";
    case TMResult::TM_PEER_PAD_NOT_FOUND:
        return "Peer Pad not found";
    case TMResult::TM_PIPE_OPERATE_FAILED:
        return "Pipe operate failed";
    case TMResult::TM_PROPERTY_NOT_FOUND:
        return "Property not found or support";
    case TMResult::TM_PROTOCOL_NOT_FOUND:
        return "Protocol not found or support";
    case TMResult::TM_STATE_ERROR:
        return "State error";
    case TMResult::TM_TIMEOUT:
        return "Timeout";
    default:
        return "Undefined error occurred";
    }
}

const char* TMResult::StrError(int result)
{
    return StrError(static_cast<TMResult::ID>(result));
}
