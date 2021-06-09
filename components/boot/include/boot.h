/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#ifndef __BOOT_H__
#define __BOOT_H__

#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>

#define BOOT_VER    "boot2.0"

int boot_main(int argc, char **argv);

#ifdef __cplusplus
}
#endif

#endif /* __BOOT_H__ */
