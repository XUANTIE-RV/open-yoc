/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __DEV_FS_H__
#define __DEV_FS_H__

#include <aos/aos.h>
#include <aos/kernel.h>
#include <poll.h>
#include <vfs.h>

#if !defined(AOS_COMP_VFS) || (defined(AOS_COMP_VFS) && !AOS_COMP_VFS)
#error "devfs not support."
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define DEVFS_NODE_NAME_MAX_LEN     23

typedef struct devfs_file_ops devfs_file_ops_t;

typedef enum {
    AOS_DEVFS_WQ_RD     = 0,
    AOS_DEVFS_WQ_WR,
    AOS_DEVFS_WQ_MAX,
} devfs_wait_queue_t;

typedef struct {
    char name[DEVFS_NODE_NAME_MAX_LEN + 1];
    const devfs_file_ops_t *ops;
    // aos_spinlock_t poll_lock;
    poll_notify_t poll_notify;
    struct pollfd *poll_fd;
    void *poll_arg;
} devfs_node_t;

#define AOS_DEVFS_NODE_INIT_VAL         { .name = { '\0', }, .ops = NULL, }
#define devfs_node_init(node)           do { *(node) = (devfs_node_t)AOS_DEVFS_NODE_INIT_VAL; } while (0)
#define devfs_node_is_valid(node)       ((node)->name[0] != '\0')

typedef file_t devfs_file_t;
typedef int devfs_poll_request_t;

struct devfs_file_ops {
    int (*ioctl)(devfs_file_t *file, int cmd, uintptr_t arg);
    int (*poll)(devfs_file_t *file, devfs_poll_request_t *req);
    /* TODO: complete the argument list of mmap() */
    int (*mmap)(devfs_file_t *file, ...);
    ssize_t (*read)(devfs_file_t *file, void *buf, size_t count);
    ssize_t (*write)(devfs_file_t *file, const void *buf, size_t count);
    int64_t (*lseek)(devfs_file_t *file, int64_t offset, int whence);
};

int devfs_add_node(devfs_node_t *node);
int devfs_remove_node(devfs_node_t *node);
void *devfs_file2dev(devfs_file_t *file);
mode_t devfs_file_get_mode(devfs_file_t *file);
uint64_t devfs_file_get_position(devfs_file_t *file);
void devfs_file_set_position(devfs_file_t *file, uint64_t pos);
void devfs_poll_add(devfs_node_t *node, devfs_wait_queue_t wq, devfs_poll_request_t *req);
void devfs_poll_wakeup(devfs_node_t *node, devfs_wait_queue_t wq);

static inline bool devfs_file_is_readable(devfs_file_t *file)
{
    bool ret;

    switch (devfs_file_get_mode(file) & O_ACCMODE) {
    case O_RDONLY:
    case O_RDWR:
        ret = true;
        break;
    default:
        ret = false;
        break;
    }

    return ret;
}

static inline bool devfs_file_is_writable(devfs_file_t *file)
{
    bool ret;

    switch (devfs_file_get_mode(file) & O_ACCMODE) {
    case O_WRONLY:
    case O_RDWR:
        ret = true;
        break;
    default:
        ret = false;
        break;
    }

    return ret;
}

static inline int64_t devfs_file_lseek_sized(devfs_file_t *file, uint64_t size, int64_t offset, int whence)
{
    int64_t ret;

    if (size > (uint64_t)INT64_MAX)
        return -EINVAL;

    switch (whence) {
    case SEEK_SET:
        if (offset < 0 || (uint64_t)offset > size)
            return -EINVAL;
        ret = offset;
        break;
    case SEEK_CUR:
        {
            uint64_t cur = devfs_file_get_position(file);
            if ((offset > 0 && cur + (uint64_t)offset > size) || (offset < 0 && (uint64_t)(-offset) > cur))
                return -EINVAL;
            ret = (int64_t)cur + offset;
        }
        break;
    case SEEK_END:
        if (offset > 0 || (uint64_t)(-offset) > size)
            return -EINVAL;
        ret = (int64_t)size + offset;
        break;
    default:
        return -EINVAL;
    }

    devfs_file_set_position(file, (uint64_t)ret);

    return ret;
}

#ifdef __cplusplus
}
#endif

#endif /* __DEV_FS_H__ */

