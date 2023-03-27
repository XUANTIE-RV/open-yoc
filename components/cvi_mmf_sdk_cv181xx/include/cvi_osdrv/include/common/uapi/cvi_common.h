/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: include/cvi_common.h
 * Description: Common video definitions.
 */

#ifndef __CVI_COMMON_H__
#define __CVI_COMMON_H__

#include <cvi_type.h>
#include <cvi_defines.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#ifndef VER_X
#define VER_X 1
#endif

#ifndef VER_Y
#define VER_Y 0
#endif

#ifndef VER_Z
#define VER_Z 0
#endif

#ifndef __FILENAME__
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

#ifdef CVI_DEBUG
#define VER_D " Debug"
#else
#define VER_D " Release"
#endif


#define ATTRIBUTE  __attribute__((aligned(ALIGN_NUM)))

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#endif

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)
#define MK_VERSION(x, y, z) STR(x) "." STR(y) "." STR(z)



typedef CVI_S32 AI_CHN;
typedef CVI_S32 AO_CHN;
typedef CVI_S32 AENC_CHN;
typedef CVI_S32 ADEC_CHN;
typedef CVI_S32 AUDIO_DEV;
typedef CVI_S32 VI_DEV;
typedef CVI_S32 VI_PIPE;
typedef CVI_S32 VI_CHN;
typedef CVI_S32 VO_DEV;
typedef CVI_S32 VO_LAYER;
typedef CVI_S32 VO_CHN;
typedef CVI_S32 GRAPHIC_LAYER;
typedef CVI_S32 VENC_CHN;
typedef CVI_S32 VDEC_CHN;
typedef CVI_S32 ISP_DEV;
typedef CVI_S32 SENSOR_ID;
typedef CVI_S32 MIPI_DEV;
typedef CVI_S32 SLAVE_DEV;
typedef CVI_S32 VPSS_GRP;
typedef CVI_S32 VPSS_CHN;

#define CVI_INVALID_CHN (-1)
#define CVI_INVALID_LAYER (-1)
#define CVI_INVALID_DEV (-1)
#define CVI_INVALID_HANDLE (-1)
#define CVI_INVALID_VALUE (-1)
#define CVI_INVALID_TYPE (-1)


#define CCM_MATRIX_SIZE             (9)
#define CCM_MATRIX_NUM              (7)


#define FOREACH_MOD(MOD) {\
	MOD(BASE)   \
	MOD(VB)	    \
	MOD(SYS)    \
	MOD(RGN)    \
	MOD(CHNL)   \
	MOD(VDEC)   \
	MOD(VPSS)   \
	MOD(VENC)   \
	MOD(H264E)  \
	MOD(JPEGE)  \
	MOD(MPEG4E) \
	MOD(H265E)  \
	MOD(JPEGD)  \
	MOD(VO)	    \
	MOD(VI)	    \
	MOD(DIS)    \
	MOD(RC)	    \
	MOD(AIO)    \
	MOD(AI)	    \
	MOD(AO)	    \
	MOD(AENC)   \
	MOD(ADEC)   \
	MOD(AUD)   \
	MOD(VPU)    \
	MOD(ISP)    \
	MOD(IVE)    \
	MOD(USER)   \
	MOD(PROC)   \
	MOD(LOG)    \
	MOD(H264D)  \
	MOD(GDC)    \
	MOD(PHOTO)  \
	MOD(FB)	    \
	MOD(BUTT)   \
}

#define GENERATE_ENUM(ENUM) CVI_ID_ ## ENUM,

typedef enum _MOD_ID_E FOREACH_MOD(GENERATE_ENUM) MOD_ID_E;

typedef struct _MMF_CHN_S {
	MOD_ID_E    enModId;
	CVI_S32     s32DevId;
	CVI_S32     s32ChnId;
} MMF_CHN_S;


/* We just copy this value of payload type from RTP/RTSP definition */
typedef enum {
	PT_PCMU          = 0,
	PT_1016          = 1,
	PT_G721          = 2,
	PT_GSM           = 3,
	PT_G723          = 4,
	PT_DVI4_8K       = 5,
	PT_DVI4_16K      = 6,
	PT_LPC           = 7,
	PT_PCMA          = 8,
	PT_G722          = 9,
	PT_S16BE_STEREO  = 10,
	PT_S16BE_MONO    = 11,
	PT_QCELP         = 12,
	PT_CN            = 13,
	PT_MPEGAUDIO     = 14,
	PT_G728          = 15,
	PT_DVI4_3        = 16,
	PT_DVI4_4        = 17,
	PT_G729          = 18,
	PT_G711A         = 19,
	PT_G711U         = 20,
	PT_G726          = 21,
	PT_G729A         = 22,
	PT_LPCM          = 23,
	PT_CelB          = 25,
	PT_JPEG          = 26,
	PT_CUSM          = 27,
	PT_NV            = 28,
	PT_PICW          = 29,
	PT_CPV           = 30,
	PT_H261          = 31,
	PT_MPEGVIDEO     = 32,
	PT_MPEG2TS       = 33,
	PT_H263          = 34,
	PT_SPEG          = 35,
	PT_MPEG2VIDEO    = 36,
	PT_AAC           = 37,
	PT_WMA9STD       = 38,
	PT_HEAAC         = 39,
	PT_PCM_VOICE     = 40,
	PT_PCM_AUDIO     = 41,
	PT_MP3           = 43,
	PT_ADPCMA        = 49,
	PT_AEC           = 50,
	PT_X_LD          = 95,
	PT_H264          = 96,
	PT_D_GSM_HR      = 200,
	PT_D_GSM_EFR     = 201,
	PT_D_L8          = 202,
	PT_D_RED         = 203,
	PT_D_VDVI        = 204,
	PT_D_BT656       = 220,
	PT_D_H263_1998   = 221,
	PT_D_MP1S        = 222,
	PT_D_MP2P        = 223,
	PT_D_BMPEG       = 224,
	PT_MP4VIDEO      = 230,
	PT_MP4AUDIO      = 237,
	PT_VC1           = 238,
	PT_JVC_ASF       = 255,
	PT_D_AVI         = 256,
	PT_DIVX3         = 257,
	PT_AVS             = 258,
	PT_REAL8         = 259,
	PT_REAL9         = 260,
	PT_VP6             = 261,
	PT_VP6F             = 262,
	PT_VP6A             = 263,
	PT_SORENSON          = 264,
	PT_H265          = 265,
	PT_VP8             = 266,
	PT_MVC             = 267,
	PT_PNG           = 268,
	/* add by ourselves */
	PT_AMR           = 1001,
	PT_MJPEG         = 1002,
	PT_BUTT
} PAYLOAD_TYPE_E;

#define VERSION_NAME_MAXLEN 128
typedef struct _MMF_VERSION_S {
	char version[VERSION_NAME_MAXLEN];
} MMF_VERSION_S;

enum CVI_GPIO_NUM_E {
	CVI_GPIOE_00 = 380,
	CVI_GPIOE_01,   CVI_GPIOE_02,   CVI_GPIOE_03,   CVI_GPIOE_04,   CVI_GPIOE_05,
	CVI_GPIOE_06,   CVI_GPIOE_07,   CVI_GPIOE_08,   CVI_GPIOE_09,	CVI_GPIOE_10,
	CVI_GPIOE_11,	CVI_GPIOE_12,	CVI_GPIOE_13,	CVI_GPIOE_14,   CVI_GPIOE_15,
	CVI_GPIOE_16,   CVI_GPIOE_17,   CVI_GPIOE_18,   CVI_GPIOE_19,   CVI_GPIOE_20,
	CVI_GPIOD_00 = 404,
	CVI_GPIOD_01,   CVI_GPIOD_02,   CVI_GPIOD_03,   CVI_GPIOD_04,   CVI_GPIOD_05,
	CVI_GPIOD_06,   CVI_GPIOD_07,   CVI_GPIOD_08,   CVI_GPIOD_09,   CVI_GPIOD_10,
	CVI_GPIOD_11,
	CVI_GPIOC_00 = 416,
	CVI_GPIOC_01,   CVI_GPIOC_02,   CVI_GPIOC_03,   CVI_GPIOC_04,   CVI_GPIOC_05,
	CVI_GPIOC_06,   CVI_GPIOC_07,   CVI_GPIOC_08,   CVI_GPIOC_09,   CVI_GPIOC_10,
	CVI_GPIOC_11,   CVI_GPIOC_12,   CVI_GPIOC_13,   CVI_GPIOC_14,   CVI_GPIOC_15,
	CVI_GPIOC_16,   CVI_GPIOC_17,   CVI_GPIOC_18,   CVI_GPIOC_19,   CVI_GPIOC_20,
	CVI_GPIOC_21,   CVI_GPIOC_22,   CVI_GPIOC_23,   CVI_GPIOC_24,   CVI_GPIOC_25,
	CVI_GPIOC_26,   CVI_GPIOC_27,   CVI_GPIOC_28,   CVI_GPIOC_29,   CVI_GPIOC_30,
	CVI_GPIOC_31,
	CVI_GPIOB_00 = 448,
	CVI_GPIOB_01,   CVI_GPIOB_02,   CVI_GPIOB_03,   CVI_GPIOB_04,   CVI_GPIOB_05,
	CVI_GPIOB_06,   CVI_GPIOB_07,   CVI_GPIOB_08,   CVI_GPIOB_09,   CVI_GPIOB_10,
	CVI_GPIOB_11,   CVI_GPIOB_12,   CVI_GPIOB_13,   CVI_GPIOB_14,   CVI_GPIOB_15,
	CVI_GPIOB_16,   CVI_GPIOB_17,   CVI_GPIOB_18,   CVI_GPIOB_19,   CVI_GPIOB_20,
	CVI_GPIOB_21,   CVI_GPIOB_22,   CVI_GPIOB_23,   CVI_GPIOB_24,   CVI_GPIOB_25,
	CVI_GPIOB_26,   CVI_GPIOB_27,   CVI_GPIOB_28,   CVI_GPIOB_29,   CVI_GPIOB_30,
	CVI_GPIOB_31,
	CVI_GPIOA_00 = 480,
	CVI_GPIOA_01,   CVI_GPIOA_02,   CVI_GPIOA_03,   CVI_GPIOA_04,   CVI_GPIOA_05,
	CVI_GPIOA_06,   CVI_GPIOA_07,   CVI_GPIOA_08,   CVI_GPIOA_09,   CVI_GPIOA_10,
	CVI_GPIOA_11,   CVI_GPIOA_12,   CVI_GPIOA_13,   CVI_GPIOA_14,   CVI_GPIOA_15,
	CVI_GPIOA_16,   CVI_GPIOA_17,   CVI_GPIOA_18,   CVI_GPIOA_19,   CVI_GPIOA_20,
	CVI_GPIOA_21,   CVI_GPIOA_22,   CVI_GPIOA_23,   CVI_GPIOA_24,   CVI_GPIOA_25,
	CVI_GPIOA_26,   CVI_GPIOA_27,   CVI_GPIOA_28,   CVI_GPIOA_29,   CVI_GPIOA_30,
	CVI_GPIOA_31,
};

struct gpio_desc_s {
	enum CVI_GPIO_NUM_E     gpio_num;
	unsigned int            active;
};


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif  /* __CVI_COMMON_H__ */
