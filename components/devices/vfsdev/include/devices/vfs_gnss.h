#ifndef __DEV_VFS_GNSS_H__
#define __DEV_VFS_GNSS_H__

#ifdef __cplusplus
extern "C" {
#endif

#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/vfs_device.h>

#define GNSS_IOC_GETINFO             (DEV_GNSS_IOCTL_CMD_BASE + 1)

#endif

#ifdef __cplusplus
}
#endif

#endif
