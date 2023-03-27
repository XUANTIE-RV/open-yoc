/*
 * Copyright (C) 2018-2022 Alibaba Group Holding Limited
 */

#ifndef __EXT4_VFS_H__
#define __EXT4_VFS_H__

#include <aos/aos.h>
#include <aos/kernel.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CONFIG_EXT4_MOUNTPOINT
#define EXT4_MOUNTPOINT CONFIG_EXT4_MOUNTPOINT
#else
#define EXT4_MOUNTPOINT "/mnt/emmc"
#endif

#ifdef CONFIG_EXT4_PARTITION_NAME
#define EXT4_PARTITION_NAME CONFIG_EXT4_PARTITION_NAME
#else
#define EXT4_PARTITION_NAME "ext4"
#endif

/**
 * @brief  register file system of ext4
 * @return 0 on success
 */
int vfs_ext4_register(void);
int vfs_ext4_unregister(void);

#ifdef __cplusplus
}
#endif

#endif /* __EXT4_VFS_H__ */

