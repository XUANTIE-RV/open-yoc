/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __LITTLEFS_VFS_H__
#define __LITTLEFS_VFS_H__

#include <aos/aos.h>
#include <aos/kernel.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  register little file system
 * @param  [in] partition_desc: partion table
 * @return 0 on success
 */
int32_t vfs_lfs_register(char *partition_desc);

/**
 * @brief  unregister little file system
 * @return 0 on success
 */
int32_t vfs_lfs_unregister(void);

#ifdef __cplusplus
}
#endif

#endif /* __LITTLEFS_VFS_H__ */

