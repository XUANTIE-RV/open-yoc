#ifndef __SC031IOT_CMOS_EX_H_
#define __SC031IOT_CMOS_EX_H_

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifdef ARCH_CV182X
#include "cvi_vip_cif_uapi.h"
#else
#include "cif_uapi.h"
#endif
#include "cvi_type.h"
#include "cvi_sns_ctrl.h"

#ifndef UNUSED
#define UNUSED(x) ((void)(x))
#endif

#define syslog(level, fmt, ...)            \
do {                                                   \
	printf(fmt, ##__VA_ARGS__);                \
} while (0)


typedef enum _SC031IOT_MODE_E {
	SC031IOT_MODE_640X480P30 = 0,
	SC031IOT_MODE_LINEAR_NUM,
	SC031IOT_MODE_NUM
} SC031IOT_MODE_E;

typedef struct _SC031IOT_STATE_S {
	CVI_U32		u32Sexp_MAX;
} SC031IOT_STATE_S;

typedef struct _SC031IOT_MODE_S {
	ISP_WDR_SIZE_S astImg[2];
	CVI_FLOAT f32MaxFps;
	CVI_FLOAT f32MinFps;
	CVI_U32 u32HtsDef;
	CVI_U32 u32VtsDef;
	SNS_ATTR_S stExp[2];
	SNS_ATTR_LARGE_S stAgain[2];
	SNS_ATTR_LARGE_S stDgain[2];
	char name[64];
} SC031IOT_MODE_S;

/****************************************************************************
 * external variables and functions                                         *
 ****************************************************************************/

extern ISP_SNS_STATE_S *g_pastSc031iot[VI_MAX_PIPE_NUM];
extern ISP_SNS_COMMBUS_U g_aunSc031iot_BusInfo[];
extern CVI_U8 sc031iot_i2c_addr;
extern const CVI_U32 sc031iot_addr_byte;
extern const CVI_U32 sc031iot_data_byte;
extern void sc031iot_init(VI_PIPE ViPipe);
extern void sc031iot_exit(VI_PIPE ViPipe);
extern int  sc031iot_write_register(VI_PIPE ViPipe, int addr, int data);
extern int  sc031iot_read_register(VI_PIPE ViPipe, int addr);
void sc031iot_mirror_flip(VI_PIPE ViPipe, ISP_SNS_MIRRORFLIP_TYPE_E eSnsMirrorFlip);
extern int  sc031iot_probe(VI_PIPE ViPipe);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /* __SC031IOT_CMOS_EX_H_ */

