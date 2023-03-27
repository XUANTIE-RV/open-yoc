/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */
/* cs_lkm.h
 *
 * Top-level LKM configuration.
 */

/*****************************************************************************
* Copyright (c) 2016-2020 by Rambus, Inc. and/or its subsidiaries.
* All rights reserved. Unauthorized use (including, without limitation,
* distribution and copying) is strictly prohibited. All use requires,
* and is subject to, explicit written authorization and nondisclosure
* Rambus, Inc. and/or its subsidiaries
*
* For more information or support, please go to our online support system at
* https://sipsupport.rambus.com.
* In case you do not have an account for this system, please send an e-mail
* to sipsupport@rambus.com.
*****************************************************************************/

#ifndef CS_LKM_H
#define CS_LKM_H


// Driver Framework configuration
#include "cs_hwpal.h"           // HWPAL_PLATFORM_DEVICE_NAME


/*-----------------------------------------------------------------------------
 * Configuration parameters that may not be modified!
 */

// Device identification required for implementation of PCI device driver
#define LKM_DEVICE_ID             0x6018

// Device vendor identification required for implementation of PCI device driver
#define LKM_VENDOR_ID             0x10EE


/* LKM_PLATFORM_DEVICE_NAME
 *
 * Platform-specific device name that can be used for looking up
 * device properties
 */
#define LKM_PLATFORM_DEVICE_NAME      HWPAL_PLATFORM_DEVICE_NAME

/*-----------------------------------------------------------------------------
 * Configuration parameters that may be modified at top level configuration
 */

// Enables strict argument checking for input parameters
#define LKM_STRICT_ARGS_CHECK

// Choose from LOG_SEVERITY_INFO, LOG_SEVERITY_WARN, LOG_SEVERITY_CRIT
#define LKM_LOG_SEVERITY LOG_SEVERITY_CRIT


#endif // CS_LKM_H


/* end of file cs_lkm.h */
