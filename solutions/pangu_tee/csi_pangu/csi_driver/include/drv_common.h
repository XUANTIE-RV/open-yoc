/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef _DRV_COMMON_H_
#define _DRV_COMMON_H_

#include <stdint.h>
#include <drv_errno.h>

/// \details driver handle
typedef void *drv_handle_t;

#ifndef CONFIG_PARAM_NOT_CHECK
#define HANDLE_PARAM_CHK(para, err)                                                                \
    do {                                                                                           \
        if ((int32_t)para == (int32_t)NULL) {                                                      \
            return (err);                                                                          \
        }                                                                                          \
    } while (0)
#else
#define HANDLE_PARAM_CHK(para, err)
#endif

/**
\brief General power states
*/
typedef enum  {
    DRV_POWER_OFF,                        ///< Power off: no operation possible
    DRV_POWER_LOW,                        ///< Low Power mode: retain state, detect and signal wake-up events
    DRV_POWER_FULL,                       ///< Power on: full operation at maximum performance
    DRV_POWER_SUSPEND,                    ///< Power suspend: power saving operation
} csi_power_stat_e;

#endif /* _DRV_COMMON_H */

