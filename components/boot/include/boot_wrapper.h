/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#ifndef __BOOT_WRAPPER_H__
#define __BOOT_WRAPPER_H__

#include <stdint.h>
#include <stddef.h>

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

#ifdef __cplusplus
}
#endif
#endif