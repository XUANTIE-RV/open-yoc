/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef AOS_VERSION_H
#define AOS_VERSION_H
#ifdef __cplusplus
extern "C" {
#endif


/**
 * Get aos product model.
 *
 * @return  model success, 0 failure.
 */
const char *aos_get_product_model(void);

/**
 * Get aos os version.
 *
 * @return  os version success, 0 failure.
 */
char *aos_get_os_version(void);

/**
 * Get aos app version.
 *
 * @return  app version success, 0 failure.
 */
char *aos_get_app_version(void);

/**
 * Get aos kernel version.
 *
 * @return  kernel version success, 0 failure.
 */
const char *aos_get_kernel_version(void);

/**
 * Get aos device name.
 *
 * @return  device name success, 0 failure.
 */
const char *aos_get_device_name(void);


#ifdef __cplusplus
}
#endif

#endif /* AOS_VERSION_H */

