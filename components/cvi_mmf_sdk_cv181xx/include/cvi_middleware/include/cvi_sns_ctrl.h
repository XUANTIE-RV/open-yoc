#ifndef __CVI_SNS_CTRL_H__
#define __CVI_SNS_CTRL_H__

#include <cif_uapi.h>
#include <cvi_errno.h>
#include <cvi_type.h>
#include "cvi_debug.h"
#include "cvi_comm_3a.h"
#include "cvi_comm_isp.h"
#include "cvi_ae_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

typedef struct combo_dev_attr_s SNS_COMBO_DEV_ATTR_S;
typedef struct mclk_pll_s SNS_MCLK_ATTR_S;

typedef struct _SNS_ATTR_S {
	CVI_U16	u16Min;
	CVI_U16 u16Max;
	CVI_U16 u16Def;
	CVI_U16 u16Step;
} SNS_ATTR_S;

typedef struct _SNS_ATTR_LARGE_S {
	CVI_U32	u32Min;
	CVI_U32 u32Max;
	CVI_U32 u32Def;
	CVI_U32 u32Step;
} SNS_ATTR_LARGE_S;

typedef struct _ISP_SNS_STATE_S {
	CVI_BOOL     bInit;                  /* CVI_TRUE: Sensor init */
	CVI_BOOL     bSyncInit;              /* CVI_TRUE: Sync Reg init */
	CVI_U8       u8ImgMode;
	CVI_U8       u8Hdr;               /* CVI_TRUE: HDR enbale */
	WDR_MODE_E  enWDRMode;

	ISP_SNS_SYNC_INFO_S astSyncInfo[2]; /* [0]: Sensor reg info of cur-frame; [1]: Sensor reg info of pre-frame ; */

	CVI_U32      au32FL[2];              /* [0]: FullLines of cur-frame; [1]: Pre FullLines of pre-frame */
	CVI_U32      u32FLStd;               /* FullLines std */
	CVI_U32      au32WDRIntTime[4];
} ISP_SNS_STATE_S;

typedef enum _ISP_SNS_MIRRORFLIP_TYPE_E {
	ISP_SNS_NORMAL      = 0,
	ISP_SNS_MIRROR      = 1,
	ISP_SNS_FLIP        = 2,
	ISP_SNS_MIRROR_FLIP = 3,
	ISP_SNS_BUTT
} ISP_SNS_MIRRORFLIP_TYPE_E;

typedef enum _ISP_SNS_L2S_MODE_E {
	SNS_L2S_MODE_AUTO = 0,	/* sensor l2s distance varies by the inttime of sef. */
	SNS_L2S_MODE_FIX,	/* sensor l2s distance is fixed. */
} ISP_SNS_INTTIME_MODE_E;

typedef struct _MCLK_ATTR_S {
	CVI_U8 u8Mclk;
	CVI_BOOL bMclkEn;
} MCLK_ATTR_S;

typedef struct _RX_INIT_ATTR_S {
	CVI_U32 MipiDev;
	CVI_S16 as16LaneId[5];
	CVI_S8  as8PNSwap[5];
	MCLK_ATTR_S stMclkAttr;
} RX_INIT_ATTR_S;

typedef enum _SNS_BDG_MUX_MODE_E {
	SNS_BDG_MUX_NONE = 0,	/* sensor bridge mux is disabled */
	SNS_BDG_MUX_2,		/* sensor bridge mux 2 input */
	SNS_BDG_MUX_3,		/* sensor bridge mux 3 input */
	SNS_BDG_MUX_4,		/* sensor bridge mux 4 input */
} SNS_BDG_MUX_MODE_E;

typedef struct _ISP_INIT_ATTR_S {
	CVI_U32 u32ExpTime;
	CVI_U32 u32AGain;
	CVI_U32 u32DGain;
	CVI_U32 u32ISPDGain;
	CVI_U32 u32Exposure;
	CVI_U32 u32LinesPer500ms;
	CVI_U32 u32PirisFNO;
	CVI_U16 u16WBRgain;
	CVI_U16 u16WBGgain;
	CVI_U16 u16WBBgain;
	CVI_U16 u16SampleRgain;
	CVI_U16 u16SampleBgain;
	CVI_U16 u16UseHwSync;
	ISP_SNS_GAIN_MODE_E enGainMode;
	ISP_SNS_INTTIME_MODE_E enL2SMode;
	SNS_BDG_MUX_MODE_E enSnsBdgMuxMode;
} ISP_INIT_ATTR_S;

typedef struct _ISP_SNS_OBJ_S {
	CVI_S32 (*pfnRegisterCallback)(VI_PIPE ViPipe, ALG_LIB_S *, ALG_LIB_S *);
	CVI_S32 (*pfnUnRegisterCallback)(VI_PIPE ViPipe, ALG_LIB_S *, ALG_LIB_S *);
	CVI_S32 (*pfnSetBusInfo)(VI_PIPE ViPipe, ISP_SNS_COMMBUS_U unSNSBusInfo);
	CVI_VOID (*pfnStandby)(VI_PIPE ViPipe);
	CVI_VOID (*pfnRestart)(VI_PIPE ViPipe);
	CVI_VOID (*pfnMirrorFlip)(VI_PIPE ViPipe, ISP_SNS_MIRRORFLIP_TYPE_E eSnsMirrorFlip);
	CVI_S32 (*pfnWriteReg)(VI_PIPE ViPipe, CVI_S32 s32Addr, CVI_S32 s32Data);
	CVI_S32 (*pfnReadReg)(VI_PIPE ViPipe, CVI_S32 s32Addr);
	CVI_S32 (*pfnSetInit)(VI_PIPE ViPipe, ISP_INIT_ATTR_S *);
	CVI_S32 (*pfnPatchRxAttr)(RX_INIT_ATTR_S *);
	CVI_VOID (*pfnPatchI2cAddr)(CVI_S32 s32I2cAddr);
	CVI_S32 (*pfnGetRxAttr)(VI_PIPE ViPipe, SNS_COMBO_DEV_ATTR_S *);
	CVI_S32 (*pfnExpSensorCb)(ISP_SENSOR_EXP_FUNC_S *);
	CVI_S32 (*pfnExpAeCb)(AE_SENSOR_EXP_FUNC_S *);
	CVI_S32 (*pfnSnsProbe)(VI_PIPE ViPipe);
} ISP_SNS_OBJ_S;

typedef enum _SNS_TYPE_E {
	SNS_TYPE_NONE = 0,
	/* ------ LINEAR BEGIN ------*/
	SONY_IMX327_MIPI_2M_30FPS_12BIT,
	SONY_IMX307_MIPI_2M_30FPS_12BIT,
	SONY_IMX307_2L_MIPI_2M_30FPS_12BIT,
	SONY_IMX307_SLAVE_MIPI_2M_30FPS_12BIT,
	GCORE_GC1054_MIPI_1M_30FPS_10BIT,
	GCORE_GC2053_MIPI_2M_30FPS_10BIT,
	GCORE_GC2053_1L_MIPI_2M_30FPS_10BIT,
	GCORE_GC2093_MIPI_2M_30FPS_10BIT,
	GCORE_GC02M1_MIPI_2M_30FPS_10BIT,
	GCORE_GC02M1_SLAVE_MIPI_2M_30FPS_10BIT,
	GCORE_GC02M1_MIPI_600P_30FPS_10BIT,
	GCORE_GC02M1_SLAVE_MIPI_600P_30FPS_10BIT,
	GCORE_GC4653_MIPI_4M_30FPS_10BIT,
	SMS_SC200AI_MIPI_2M_30FPS_10BIT,
	SMS_SC2336_MIPI_2M_30FPS_10BIT,
	/* ------ LINEAR END ------*/
	SNS_TYPE_LINEAR_BUTT,
	/* ------ WDR 2TO1 BEGIN ------*/
	SONY_IMX327_MIPI_2M_30FPS_12BIT_WDR2TO1 = SNS_TYPE_LINEAR_BUTT,
	SONY_IMX307_MIPI_2M_30FPS_12BIT_WDR2TO1,
	SONY_IMX307_2L_MIPI_2M_30FPS_12BIT_WDR2TO1,
	SONY_IMX307_SLAVE_MIPI_2M_30FPS_12BIT_WDR2TO1,
	GCORE_GC2093_MIPI_2M_30FPS_10BIT_WDR2TO1,
	SMS_SC200AI_MIPI_2M_30FPS_10BIT_WDR2TO1,
	/* ------ WDR 2TO1 END ------*/
	SNS_TYPE_WDR_BUTT,
} SNS_TYPE_E;

#if CONFIG_SENSOR_GCORE_GC1054
extern ISP_SNS_OBJ_S stSnsGc1054_Obj;
#endif
#if CONFIG_SENSOR_GCORE_GC2053
extern ISP_SNS_OBJ_S stSnsGc2053_Obj;
#endif
#if CONFIG_SENSOR_GCORE_GC2053_1L
extern ISP_SNS_OBJ_S stSnsGc2053_1l_Obj;
#endif
#if CONFIG_SENSOR_GCORE_GC2093
extern ISP_SNS_OBJ_S stSnsGc2093_Obj;
#endif
#if CONFIG_SENSOR_GCORE_GC02M1
extern ISP_SNS_OBJ_S stSnsGc02m1_Obj;
#endif
#if CONFIG_SENSOR_GCORE_GC02M1_SLAVE
extern ISP_SNS_OBJ_S stSnsGc02m1_Slave_Obj;
#endif
#if CONFIG_SENSOR_GCORE_GC4653
extern ISP_SNS_OBJ_S stSnsGc4653_Obj;
#endif
#if CONFIG_SENSOR_SMS_SC200AI
extern ISP_SNS_OBJ_S stSnsSC200AI_Obj;
#endif
#if CONFIG_SENSOR_SMS_SC2336
extern ISP_SNS_OBJ_S stSnsSC2336_Obj;
#endif
#if CONFIG_SENSOR_SONY_IMX307
extern ISP_SNS_OBJ_S stSnsImx307_Obj;
#endif
#if CONFIG_SENSOR_SONY_IMX307_2L
extern ISP_SNS_OBJ_S stSnsImx307_2l_Obj;
#endif
#if CONFIG_SENSOR_SONY_IMX307_SLAVE
extern ISP_SNS_OBJ_S stSnsImx307_Slave_Obj;
#endif
#if CONFIG_SENSOR_SONY_IMX327
extern ISP_SNS_OBJ_S stSnsImx327_Obj;
#endif

#define CMOS_CHECK_POINTER(ptr)\
	do {\
		if (ptr == CVI_NULL) {\
			syslog(LOG_ERR, "Null Pointer!\n");\
			return CVI_ERR_VI_INVALID_NULL_PTR;\
		} \
	} while (0)

#define CMOS_CHECK_POINTER_VOID(ptr)\
	do {\
		if (ptr == CVI_NULL) {\
			syslog(LOG_ERR, "Null Pointer!\n");\
			return;\
		} \
	} while (0)

#define SENSOR_FREE(ptr)\
	do {\
		if (ptr != CVI_NULL) {\
			free(ptr);\
			ptr = CVI_NULL;\
		} \
	} while (0)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* __CVI_SNS_CTRL_H__ */
