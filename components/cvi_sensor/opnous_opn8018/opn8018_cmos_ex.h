#ifndef __OPN8018_CMOS_EX_H_
#define __OPN8018_CMOS_EX_H_

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

#define syslog(level, fmt, ...)            \
do {                                                   \
	printf(fmt, ##__VA_ARGS__);                \
} while (0)

#ifndef UNUSED
#define UNUSED(x) ((void)(x))
#endif

enum opn8018_linear_regs_e {
	LINEAR_EXP_0 = 0,
	LINEAR_EXP_1,
	LINEAR_EXP_2,
	LINEAR_AGAIN0,
	LINEAR_AGAIN1,
	LINEAR_VTS_0,
	LINEAR_VTS_1,
	LINEAR_REGS_NUM
};

typedef enum _OPN8018_MODE_E {
	OPN8018_MODE_640X480P120 = 0,
	OPN8018_MODE_LINEAR_NUM,
	OPN8018_MODE_NUM
} OPN8018_MODE_E;

typedef struct _OPN8018_STATE_S {
	CVI_U32		u32Sexp_MAX;
} OPN8018_STATE_S;

typedef struct _OPN8018_MODE_S {
	ISP_WDR_SIZE_S astImg[2];
	CVI_FLOAT f32MaxFps;
	CVI_FLOAT f32MinFps;
	CVI_U32 u32HtsDef;
	CVI_U32 u32VtsDef;
	CVI_U16 u16L2sOffset;
	CVI_U16 u16TopBoundary;
	CVI_U16 u16BotBoundary;
	SNS_ATTR_S stExp[2];
	SNS_ATTR_S stAgain[2];
	SNS_ATTR_S stDgain[2];
	CVI_U32 u32L2S_MAX;
	char name[64];
} OPN8018_MODE_S;

/****************************************************************************
 * external variables and functions                                         *
 ****************************************************************************/

extern ISP_SNS_STATE_S *g_pastOpn8018[VI_MAX_PIPE_NUM];
extern ISP_SNS_COMMBUS_U g_aunOpn8018_BusInfo[];
extern CVI_U16 g_au16Opn8018_GainMode[];
extern CVI_U16 g_au16Opn8018_L2SMode[VI_MAX_PIPE_NUM];
extern CVI_U8 opn8018_i2c_addr;
extern const CVI_U32 opn8018_addr_byte;
extern const CVI_U32 opn8018_data_byte;
extern void opn8018_init(VI_PIPE ViPipe);
extern void opn8018_exit(VI_PIPE ViPipe);
extern void opn8018_standby(VI_PIPE ViPipe);
extern void opn8018_restart(VI_PIPE ViPipe);
extern int  opn8018_write_register(VI_PIPE ViPipe, int addr, int data);
extern int  opn8018_read_register(VI_PIPE ViPipe, int addr);
extern void opn8018_mirror_flip(VI_PIPE ViPipe, ISP_SNS_MIRRORFLIP_TYPE_E eSnsMirrorFlip);
extern int opn8018_probe(VI_PIPE ViPipe);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /* __OPN8018_CMOS_EX_H_ */
