/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#ifndef __BOOT_WRAPPER_H__
#define __BOOT_WRAPPER_H__

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
  * load and jump to next image
*/
/* TODO weak */
void boot_load_and_jump(void);

/**
  * system reboot
*/
/* TODO weak */
void boot_sys_reboot(void);

/**
 * @brief  check the image is needed ota or not
 * @param name partition name
 * @return true: no needed ota, false: need ota
 */
/* TODO weak */
bool boot_is_no_needed_ota(const char *name);

#ifdef __cplusplus
}
#endif
#endif