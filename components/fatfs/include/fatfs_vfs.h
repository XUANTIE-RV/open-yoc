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

#ifdef CONFIG_SD_FATFS_MOUNTPOINT
#define SD_FATFS_MOUNTPOINT CONFIG_SD_FATFS_MOUNTPOINT
#else
#define SD_FATFS_MOUNTPOINT "/mnt/sd"
#endif

/**
 * @brief  register file system of fat
 * @return 0 on success
 */
int vfs_fatfs_register(void);
int vfs_fatfs_unregister(void);

#ifdef __cplusplus
}
#endif

#endif /* __FATFS_VFS_H__ */

