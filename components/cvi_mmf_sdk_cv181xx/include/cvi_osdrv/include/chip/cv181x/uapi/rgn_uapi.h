#ifndef __U_RGN_UAPI_H__
#define __U_RGN_UAPI_H__

#include "rtos_types.h"
#ifdef __cplusplus
	extern "C" {
#endif


#define RGN_IOC_MAGIC		'V'
#define RGN_IOC_BASE		0x20

#define RGN_IOC_G_CTRL		(RGN_IOC_BASE)
#define RGN_IOC_S_CTRL		(RGN_IOC_BASE + 1)

enum RNG_IOCTL {
	RGN_IOCTL_SC_SET_RGN,
	RGN_IOCTL_DISP_SET_RGN,
	RGN_IOCTL_SDK_CTRL,
	RGN_IOCTL_MAX,
};

enum RGN_SDK_CTRL {
	RGN_SDK_CREATE,
	RGN_SDK_DESTORY,
	RGN_SDK_GET_ATTR,
	RGN_SDK_SET_ATTR,
	RGN_SDK_SET_BIT_MAP,
	RGN_SDK_ATTACH_TO_CHN,
	RGN_SDK_DETACH_FROM_CHN,
	RGN_SDK_SET_DISPLAY_ATTR,
	RGN_SDK_GET_DISPLAY_ATTR,
	RGN_SDK_GET_CANVAS_INFO,
	RGN_SDK_UPDATE_CANVAS,
	RGN_SDK_INVERT_COLOR,
	RGN_SDK_SET_CHN_PALETTE,
	RGN_SDK_MAX,
};

struct rgn_ext_control {
	__u32 id;
	__u32 sdk_id;
	__u32 handle;
	void *ptr1;
	void *ptr2;
} __attribute__ ((packed));

struct rgn_plane {
	__u64 addr;
};

/*
 * @index:
 * @length: length of planes
 * @planes: to describe buf
 * @reserved
 */
struct rgn_buffer {
	__u32 index;
	__u32 length;
	struct rgn_plane planes[3];
	__u32 reserved;
};

struct rgn_event {
	__u32			dev_id;
	__u32			type;
	__u32			frame_sequence;
	struct timeval		timestamp;
};

int rgn_open(void);
int rgn_close(void);
long rgn_ioctl(u_int cmd, u_long arg);
int rgn_core_init(void);
int rgn_core_deinit(void);

#ifdef __cplusplus
	}
#endif

#endif /* __U_RGN_UAPI_H__ */
