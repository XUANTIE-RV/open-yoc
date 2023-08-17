#ifndef __SC030IOT_CMOS_EX_H_
#define __SC030IOT_CMOS_EX_H_

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


typedef enum _SC030IOT_MODE_E {
	SC030IOT_MODE_640X480P30 = 0,
	SC030IOT_MODE_LINEAR_NUM,
	SC030IOT_MODE_NUM
} SC030IOT_MODE_E;

typedef struct _SC030IOT_STATE_S {
	CVI_U32		u32Sexp_MAX;
} SC030IOT_STATE_S;

typedef struct _SC030IOT_MODE_S {
	ISP_WDR_SIZE_S astImg[2];
	CVI_FLOAT f32MaxFps;
	CVI_FLOAT f32MinFps;
	CVI_U32 u32HtsDef;
	CVI_U32 u32VtsDef;
	SNS_ATTR_S stExp[2];
	SNS_ATTR_LARGE_S stAgain[2];
	SNS_ATTR_LARGE_S stDgain[2];
	char name[64];
} SC030IOT_MODE_S;

/****************************************************************************
 * external variables and functions                                         *
 ****************************************************************************/

extern ISP_SNS_STATE_S *g_pastSc030iot[VI_MAX_PIPE_NUM];
extern ISP_SNS_COMMBUS_U g_aunSc030iot_BusInfo[];
extern CVI_U8 sc030iot_i2c_addr;
extern const CVI_U32 sc030iot_addr_byte;
extern const CVI_U32 sc030iot_data_byte;
extern void sc030iot_init(VI_PIPE ViPipe);
extern void sc030iot_exit(VI_PIPE ViPipe);
extern int  sc030iot_write_register(VI_PIPE ViPipe, int addr, int data);
extern int  sc030iot_read_register(VI_PIPE ViPipe, int addr);
extern int  sc030iot_probe(VI_PIPE ViPipe);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /* __SC030IOT_CMOS_EX_H_ */

