/*
 * Copyright (C) 2020 FishSemi Inc. All rights reserved.
 */

/******************************************************************************
 * @file     openamp.h
 * @brief    head file for openamp
 * @version  V1.0
 * @date     20. Jan 2020
 ******************************************************************************/

#ifndef _OPENAMP_H
#define _OPENAMP_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <openamp/open_amp.h>
#include <openamp/remoteproc_loader.h>

/****************************************************************************
 * Public Types
 ****************************************************************************/

typedef void (*rpmsg_dev_cb_t)(struct rpmsg_device *rdev, void *priv);
typedef void (*rpmsg_bind_cb_t)(struct rpmsg_device *rdev, void *priv,
                                const char *name, uint32_t dest);

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

const char *rpmsg_get_cpuname(struct rpmsg_device *rdev);
int rpmsg_register_callback(void *priv,
                            rpmsg_dev_cb_t device_created,
                            rpmsg_dev_cb_t device_destroy,
                            rpmsg_bind_cb_t ns_bind);
void rpmsg_unregister_callback(void *priv,
                               rpmsg_dev_cb_t device_created,
                               rpmsg_dev_cb_t device_destroy,
                               rpmsg_bind_cb_t ns_bind);

#ifdef __cplusplus
}
#endif
#endif /*_OPENAMP_H */
