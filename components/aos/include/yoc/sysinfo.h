/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef YOC_SYSINFO_H
#define YOC_SYSINFO_H

#include <aos/version.h>


#ifdef __cplusplus
extern "C" {
#endif

/**
 * Get aos device id(id2/cid).
 *
 * @return  device name success, 0 failure.
 */
char *aos_get_device_id(void);

/**
 * Get yoc get imsi.
 *
 * @return  imsi string.
 */
const char *aos_get_imsi(void);

/**
 * Get yoc get imei.
 *
 * @return  imei string.
 */
const char *aos_get_imei(void);


#ifdef __cplusplus
}
#endif

#endif /* AOS_VERSION_H */

