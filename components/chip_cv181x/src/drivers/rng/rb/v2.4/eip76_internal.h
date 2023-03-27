/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/* eip76_internal.h
 *
 * EIP-76 Driver Library Internal Macros
 */



#ifndef EIP76_INTERNAL_H_
#define EIP76_INTERNAL_H_

/*----------------------------------------------------------------------------
 * This module uses (requires) the following interface(s):
 */

// Driver Framework Basic Definitions API
#include "basic_defs.h"         // uint8_t, uint32_t

// Driver Framework Device API
#include "device_types.h"       // Device_Handle_t

// EIP-76 Driver Library Types API
#include "eip76_types.h"        // EIP76_* types

// EIP-76 Driver Library Internal interfaces
#include "eip76_fsm.h"          // State machine

/*----------------------------------------------------------------------------
 * Definitions and macros
 */


// I/O Area, used internally
typedef struct
{
    Device_Handle_t Device;
    uint32_t SavedControl;
    uint32_t PRM_WordCount;
    uint32_t State;
    uint32_t ConfigStatus;
    uint32_t Index;
    bool     Flag;
} EIP76_True_IOArea_t;

#define IOAREA(_p) ((volatile EIP76_True_IOArea_t *)_p)

#ifdef EIP76_STRICT_ARGS
#define EIP76_CHECK_POINTER(_p) \
    if (NULL == (_p)) \
        return EIP76_ARGUMENT_ERROR;
#define EIP76_CHECK_INT_INRANGE(_i, _min, _max) \
    if ((_i) < (_min) || (_i) > (_max)) \
        return EIP76_ARGUMENT_ERROR;
#define EIP76_CHECK_INT_ATLEAST(_i, _min) \
    if ((_i) < (_min)) \
        return EIP76_ARGUMENT_ERROR;
#define EIP76_CHECK_INT_ATMOST(_i, _max) \
    if ((_i) > (_max)) \
        return EIP76_ARGUMENT_ERROR;
#else
/* EIP76_STRICT_ARGS undefined */
#define EIP76_CHECK_POINTER(_p)
#define EIP76_CHECK_INT_INRANGE(_i, _min, _max)
#define EIP76_CHECK_INT_ATLEAST(_i, _min)
#define EIP76_CHECK_INT_ATMOST(_i, _max)
#endif /*end of EIP76_STRICT_ARGS */

#define TEST_SIZEOF(type, size) \
    extern int size##_must_bigger[1 - 2*((int)(sizeof(type) > size))]

// validate the size of the fake and real IOArea structures
TEST_SIZEOF(EIP76_True_IOArea_t, EIP76_IOAREA_REQUIRED_SIZE);


#endif /* EIP76_INTERNAL_H_ */

/* end of file eip76_internal.h */
