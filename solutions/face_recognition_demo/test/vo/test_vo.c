#include "board.h"
#include <aos/kernel.h>
#include <stdio.h>
#include <ulog/ulog.h>
#include <aos/cli.h>
#include "fatfs_vfs.h"
#include "vfs.h"
#include <cvi_base.h>
#include "vo_uapi.h"
#include "cvi_vb.h"
#include "cvi_sys.h"
#include "cvi_vo.h"
#include "cvi_buffer.h"
#include "test_vo.h"
#include "cvi_comm_vo.h"
#include <pinctrl-mars.h>

void start_vo(int32_t argc, char **argv)
{
	//vo init
	VO_CONFIG_S stVoConfig = { 0 };
	VO_LAYER VoLayer = 0;
	VO_VIDEO_LAYER_ATTR_S stLayerAttr = { { 0 } };
	RECT_S stDefDispRect  = {0, 0, 720, 1280};
	SIZE_S stDefImageSize = {720, 1280};
	VO_CHN_ATTR_S stVOChnAttr = { 0 };
	MMF_CHN_S stSrcChn;
	MMF_CHN_S stDestChn;

	stVoConfig.VoDev	 = 0;
	stVoConfig.u32DisBufLen  = 3;
	stVoConfig.stVoPubAttr.enIntfType  = VO_INTF_MIPI;
	stVoConfig.stVoPubAttr.enIntfSync  = VO_OUTPUT_720x1280_60;
	stVoConfig.stVoPubAttr.u32BgColor = COLOR_10_RGB_BLACK;
	stVoConfig.stDispRect	 = stDefDispRect;
	stVoConfig.stImageSize	 = stDefImageSize;
	stVoConfig.enPixFormat	 = PIXEL_FORMAT_NV21;
	stLayerAttr.stDispRect = stDefDispRect;
	stLayerAttr.u32DispFrmRt = 60;
	stLayerAttr.stImageSize = stDefImageSize;
	stLayerAttr.enPixFormat = stVoConfig.enPixFormat;
	stVOChnAttr.stRect = stLayerAttr.stDispRect;

	APP_CHECK_RET(CVI_VO_SetPubAttr(stVoConfig.VoDev, &stVoConfig.stVoPubAttr),
			"CVI_VO_SetPubAttr failed!\n");
	APP_CHECK_RET(CVI_VO_Enable(stVoConfig.VoDev), "CVI_VO_Enable failed!\n");
	APP_CHECK_RET(CVI_VO_SetDisplayBufLen(VoLayer, stVoConfig.u32DisBufLen),
			"CVI_VO_SetDisplayBufLen failed!\n");
	APP_CHECK_RET(CVI_VO_SetVideoLayerAttr(VoLayer, &stLayerAttr),
			"CVI_VO_SetVideoLayerAttr failed!\n");
	APP_CHECK_RET(CVI_VO_EnableVideoLayer(VoLayer),
			"CVI_VO_EnableVideoLayer failed!\n");
	APP_CHECK_RET(CVI_VO_SetChnAttr(VoLayer, 0, &stVOChnAttr),
			"CVI_VO_SetChnAttr failed!\n");

	aos_cli_printf("******start vo******\n");
	APP_CHECK_RET(CVI_VO_EnableChn(VoLayer, 0), "CVI_VO_EnableChn failed!\n");
	// APP_CHECK_RET(CVI_VO_SetChnRotation(VoLayer, 0, ROTATION_90), "CVI_VO_SetChnRotation failed\n");

	stSrcChn.enModId = CVI_ID_VPSS;
	stSrcChn.s32DevId = 0;
	stSrcChn.s32ChnId = 0;

	stDestChn.enModId = CVI_ID_VO;
	stDestChn.s32DevId = 0;
	stDestChn.s32ChnId = 0;

	APP_CHECK_RET(CVI_SYS_Bind(&stSrcChn, &stDestChn), "CVI_SYS_Bind(VPSS-VO)");
}
ALIOS_CLI_CMD_REGISTER(start_vo, start_vo, start vo);

void stop_vo(int32_t argc, char **argv)
{
	VO_DEV VoDev = 0;
	VO_LAYER VoLayer = 0;
	VO_CHN VoChn = 0;
	MMF_CHN_S stSrcChn;
	MMF_CHN_S stDestChn;

	stSrcChn.enModId = CVI_ID_VPSS;
	stSrcChn.s32DevId = 0;
	stSrcChn.s32ChnId = 0;
	stDestChn.enModId = CVI_ID_VO;
	stDestChn.s32DevId = 0;
	stDestChn.s32ChnId = 0;

	APP_CHECK_RET(CVI_SYS_UnBind(&stSrcChn, &stDestChn), "CVI_SYS_UnBind(VPSS-VO) fail");
	APP_CHECK_RET(CVI_VO_DisableChn(VoLayer, VoChn), "CVI_VO_DisableChn failed!\n");
	APP_CHECK_RET(CVI_VO_DisableVideoLayer(VoLayer), "CVI_VO_DisableVideoLayer failed!\n");
	APP_CHECK_RET(CVI_VO_Disable(VoDev), "CVI_VO_Disable failed!\n");
	aos_cli_printf("******stop vo******\n");
}
ALIOS_CLI_CMD_REGISTER(stop_vo, stop_vo, stop vo);
