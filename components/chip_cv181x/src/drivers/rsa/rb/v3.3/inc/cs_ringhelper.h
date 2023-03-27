/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */
/* cs_ringhelper.h
 *
 * Ring Helper Configuration File
 */



#ifndef INCLUDE_GUARD_CS_RINGHELPER_H
#define INCLUDE_GUARD_CS_RINGHELPER_H

#include "cs_driver.h"

// when enabled, all function call parameters are sanity-checked
// comment-out to disable this code
#ifndef DRIVER_PERFORMANCE
#define RINGHELPER_STRICT_ARGS
#endif

// the following switch removes support for the Status Callback Function
//#define RINGHELPER_REMOVE_STATUSFUNC

// the following switch removes support for separate rings
// use when only overlapping rings are used
//#define RINGHELPER_REMOVE_SEPARATE_RING_SUPPORT


#endif /* Include Guard */


/* end of file cs_ringhelper.h */
