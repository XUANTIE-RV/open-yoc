#ifndef _MODULES_SYS_INCLUDE_SYS_IOCTL_H_
#define _MODULES_SYS_INCLUDE_SYS_IOCTL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <sys_uapi.h>

int sys_set_vivpssmode(const VI_VPSS_MODE_S *cfg);
int sys_set_vpssmode(VPSS_MODE_E val);
int sys_set_vpssmodeex(const VPSS_MODE_S *cfg);
int sys_set_sys_init();
int sys_get_vivpssmode(const VI_VPSS_MODE_S *cfg);
int sys_get_vpssmode(VPSS_MODE_E *val);
int sys_get_vpssmodeex(const VPSS_MODE_S *cfg);
int sys_get_sys_init(CVI_U32 *val);

#ifdef __cplusplus
}
#endif

#endif // _MODULES_SYS_INCLUDE_SYS_IOCTL_H_