/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

#ifndef INC_RAMBUS_ERRCODE_H
#define INC_RAMBUS_ERRCODE_H

#include <drv/common.h>
#include "rambus_log.h"

#define SC_ERROR_BASE 0x10000000

/* common */
#define SC_OK CSI_OK
#define SC_FAIL SC_ERROR_BASE + 1
#define SC_MEM_OVERFLOW SC_ERROR_BASE + 2
#define SC_PARAM_INV SC_ERROR_BASE + 3
#define SC_OPERATION_BUSY SC_ERROR_BASE + 4
#define SC_AUTH_FAIL SC_ERROR_BASE + 5
#define SC_CRYPT_FAIL SC_ERROR_BASE + 6
#define SC_NOT_SUPPORT SC_ERROR_BASE + 7
#define SC_INVALID_PADDING SC_ERROR_BASE + 8
#define SC_BAD_INPUT_DATA SC_ERROR_BASE + 9
#define SC_INVALID_KEY_LENGTH SC_ERROR_BASE + 10
#define SC_INVALID_INPUT_LENGTH SC_ERROR_BASE + 11
#define SC_FEATURE_UNAVAILABLE SC_ERROR_BASE + 12
#define SC_HW_ACCEL_FAILED SC_ERROR_BASE + 13
#define SC_CCM_AUTH_FAILED SC_ERROR_BASE + 14
#define SC_KEY_GEN_FAILED SC_ERROR_BASE + 15
#define SC_KEY_CHECK_FAILED SC_ERROR_BASE + 16
#define SC_PUBLIC_FAILED SC_ERROR_BASE + 17
#define SC_PRIVATE_FAILED SC_ERROR_BASE + 18
#define SC_VERIFY_FAILED SC_ERROR_BASE + 19
#define SC_OUTPUT_TOO_LARGE SC_ERROR_BASE + 20
#define SC_RNG_FAILED SC_ERROR_BASE + 21
#define SC_BUFFER_TOO_SMALL SC_ERROR_BASE + 22
#define SC_INVALID_FORMAT SC_ERROR_BASE + 23
#define SC_ALLOC_FAILED SC_ERROR_BASE + 24
#define SC_DRV_FAILED SC_ERROR_BASE + 25



#define CHECK_RET(x)                                                      \
        do                                                                \
        {                                                                 \
                if (!(x))                                                 \
                {                                                         \
                        LOG_CRIT("err %s, %d\n", __FUNCTION__, __LINE__); \
                        return;                                           \
                }                                                         \
        } while (0)

#define CHECK_RET_WITH_RET(x, ret)                                        \
        do                                                                \
        {                                                                 \
                if (!(x))                                                 \
                {                                                         \
                        LOG_CRIT("err %s, %d\n", __FUNCTION__, __LINE__); \
                        return ret;                                       \
                }                                                         \
        } while (0)

#define CHECK_16byte_ALIGNMENT(_i, ret)                           \
        if ((((uint64_t)_i) & 0xF) != 0)                          \
        {                                                         \
                LOG_CRIT("err %s, %d\n", __FUNCTION__, __LINE__); \
                return ret;                                       \
        }

#define CHECK_32byte_ALIGNMENT(_i, ret)                           \
        if ((((uint64_t)_i) & 0x1F) != 0)                         \
        {                                                         \
                LOG_CRIT("err %s, %d\n", __FUNCTION__, __LINE__); \
                return ret;                                       \
        }

#define CHECK_64byte_ALIGNMENT(_i, ret)                           \
        if ((((uint64_t)_i) & 0x3F) != 0)                         \
        {                                                         \
                LOG_CRIT("err %s, %d\n", __FUNCTION__, __LINE__); \
                return ret;                                       \
        }


#endif