/*
 * Copyright (C) 2020 FishSemi Inc. All rights reserved.
 */

/******************************************************************************
 * @file     misc_rpmsg.h
 * @brief    head file for misc rpmsg
 * @version  V1.0
 * @date     20. Jan 2020
 ******************************************************************************/

#ifndef _MISC_RPMSG_H
#define _MISC_RPMSG_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <string.h>

#include <openamp/open_amp.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define MISC_RETENT_ADD         1
#define MISC_RETENT_SET         2
#define MISC_REMOTE_BOOT        3
#define MISC_REMOTE_CLOCKSYNC   4
#define MISC_REMOTE_ENVSYNC     5
#define MISC_REMOTE_INFOWRITE   6
#define MISC_REMOTE_RAMFLUSH    7

/* Access macros ************************************************************/

/****************************************************************************
 * Name: MISC_RAMFLUSH_REGISTER
 *
 * Description:
 *   Register ramflush callback
 *
 * Input Parameters:
 *   dev - Device-specific state data
 *   c   - Callback registered to misc
 *
 * Returned Value:
 *   Zero on success, NULL on failure.
 *
 ****************************************************************************/

#define MISC_RAMFLUSH_REGISTER(d,c) ((d)?(d)->ops->ramflush_register((d),(c)):0)

/****************************************************************************
 * Name: MISC_RETENT_SAVE
 *
 * Description:
 *   Save ram data to flash
 *
 * Input Parameters:
 *   dev  - Device-specific state data
 *   file - File to be saved to
 *
 * Returned Value:
 *   Zero on success, NULL on failure.
 *
 ****************************************************************************/

#define MISC_RETENT_SAVE(d,f) ((d)?(d)->ops->retent_save((d),(f)):0)

/****************************************************************************
 * Name: MISC_RETENT_RESTORE
 *
 * Description:
 *   Restore from flash
 *
 * Input Parameters:
 *   dev  - Device-specific state data
 *   file - File to be restored from
 *
 * Returned Value:
 *   Zero on success, NULL on failure.
 *
 ****************************************************************************/

#define MISC_RETENT_RESTORE(d,f) ((d)?(d)->ops->retent_restore((d),(f)):0)

/****************************************************************************
 * Public Types
 ****************************************************************************/

typedef int (*misc_ramflush_cb_t)(char *fpath);

struct misc_dev_s;
struct misc_ops_s
{
    int (*ramflush_register)(struct misc_dev_s *dev, misc_ramflush_cb_t cb);
    int (*retent_save)(struct misc_dev_s *dev, char *file);
    int (*retent_restore)(struct misc_dev_s *dev, char *file);
};

struct misc_dev_s
{
    const struct misc_ops_s *ops;
};

struct misc_retent_add_s
{
    uint32_t blkid;
    void     *base;
    uint32_t size;
    bool     dma;
};

struct misc_retent_set_s
{
    uint32_t blkid;
    uint32_t flush;
};

struct misc_remote_boot_s
{
    const char *name;
};

struct misc_remote_envsync_s
{
    const char *name;
    const char *value;
};

struct misc_remote_infowrite_s
{
    const char *name;
    uint8_t    *value;
    uint32_t    len;
};

struct misc_remote_ramflush_s
{
    const char *fpath;
};

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

#ifdef __cplusplus
#define EXTERN extern "C"
extern "C"
{
#else
#define EXTERN extern
#endif

struct misc_dev_s *misc_rpmsg_initialize(const char *cpuname,
                                         bool devctl);
int misc_rpmsg_clocksync(void);

#ifdef __cplusplus
}
#endif

#endif /* _MISC_RPMSG_H */
