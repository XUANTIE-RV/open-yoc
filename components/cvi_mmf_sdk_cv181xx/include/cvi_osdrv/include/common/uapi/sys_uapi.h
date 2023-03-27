#ifndef __U_SYS_UAPI_H__
#define __U_SYS_UAPI_H__

#ifdef __cplusplus
	extern "C" {
#endif

#include "rtos_types.h"

#define SYS_ION_ALLOC     0x1
#define SYS_ION_FREE      0x2
#define SYS_CACHE_INVLD   0x3
#define SYS_CACHE_FLUSH   0x4

#define SYS_INIT_USER     0x5
#define SYS_EXIT_USER     0x6
#define SYS_GET_SYSINFO   0x7

#define SYS_SET_MODECFG   0x8
#define SYS_GET_MODECFG   0x9
#define SYS_SET_BINDCFG   0xa
#define SYS_GET_BINDCFG   0xb

#define SYS_IOC_G_CTRL    0xc
#define SYS_IOC_S_CTRL    0xd

#define SYS_READ_CHIP_ID                0xe
#define SYS_READ_CHIP_VERSION           0xf
#define SYS_READ_CHIP_PWR_ON_REASON     0x10

enum SYS_IOCTL {
	SYS_IOCTL_SET_VIVPSSMODE,
	SYS_IOCTL_GET_VIVPSSMODE,
	SYS_IOCTL_SET_VPSSMODE,
	SYS_IOCTL_GET_VPSSMODE,
	SYS_IOCTL_SET_VPSSMODE_EX,
	SYS_IOCTL_GET_VPSSMODE_EX,
	SYS_IOCTL_SET_SYS_INIT,
	SYS_IOCTL_GET_SYS_INIT,
	SYS_IOCTL_MAX,
};

struct sys_ext_control {
	__u32 id;
	__u32 reserved[1];
	union {
		__s32 value;
		__s64 value64;
		void *ptr;
	};
};

struct sys_bind_cfg {
	__u32 is_bind;
	__u32 get_by_src;
	MMF_CHN_S mmf_chn_src;
	MMF_CHN_S mmf_chn_dst;
	MMF_BIND_DEST_S bind_dst;
};

int sys_core_init(void);
int sys_core_exit(void);
int cvi_sys_open();
int cvi_sys_close();
int cvi_sys_ioctl(unsigned int cmd, void *arg);


#ifdef __cplusplus
	}
#endif

#endif /* __U_SYS_UAPI_H__ */
