/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __FATFS_VFS_H__
#define __FATFS_VFS_H__

#include <aos/aos.h>
#include <aos/kernel.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  register file system of fat
 * @return 0 on success
 */
int vfs_fatfs_register(void);

#ifdef __cplusplus
}
#endif

#endif /* __FATFS_VFS_H__ */

