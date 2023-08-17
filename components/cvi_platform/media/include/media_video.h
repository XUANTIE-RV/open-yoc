#ifndef __MEDIA_VIDEO_H__
#define __MEDIA_VIDEO_H__

#include <stdio.h>
#include "cvi_param.h"
#include "cvi_comm_venc.h"

//#define MEDIABUG_PRINTF(fmt,...) printf(fmt,##__VA_ARGS__)
#define MEDIABUG_PRINTF(fmt,...)

#define MEDIA_CHECK_RET(actual, fmt, arg...)                                   \
	do {																		 \
		if ((actual) != 0) {													 \
			MEDIABUG_PRINTF("[%d]:%s() \n" fmt, __LINE__, __func__, ## arg); \
			return -1;																\
		}																		 \
	} while (0)

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int MEDIA_VIDEO_SysVbInit(PARAM_SYS_CFG_S * pstSysCtx);
int MEDIA_VIDEO_ViInit(PARAM_VI_CFG_S * pstViCfg);
int MEDIA_VIDEO_VpssInit(PARAM_VPSS_CFG_S * pstVpssCtx);
int MEDIA_VIDEO_VencInit(PARAM_VENC_CFG_S *pstVencCfg);
int MEDIA_VIDEO_SysVbDeinit();
int MEDIA_VIDEO_ViDeinit(PARAM_VI_CFG_S * pstViCfg);
int MEDIA_VIDEO_VpssDeinit(PARAM_VPSS_CFG_S * pstVpssCtx);
int MEDIA_VIDEO_VencDeInit(PARAM_VENC_CFG_S *pstVencCfg);
int MEDIA_VIDEO_VoDeinit(PARAM_VO_CFG_S * pstVoCtx);
int MEDIA_VIDEO_SysInit();
int MEDIA_VIDEO_Init();
int MEDIA_VIDEO_VencGetStream(int VencChn,VENC_STREAM_S *pstStreamFrame,unsigned int blocktimeMs);
int MEDIA_VIDEO_VencReleaseStream(int VencChn,VENC_STREAM_S *pstStreamFrame);
int MEDIA_VIDEO_VencRequstIDR(int VencChn);
int MEDIA_VIDEO_ViSetImageMono(VI_PIPE ViPipe);
int MEDIA_VIDEO_VoInit(PARAM_VO_CFG_S * pstVoCtx);
int MEDIA_VIDEO_VoDeinit(PARAM_VO_CFG_S * pstVoCtx);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
