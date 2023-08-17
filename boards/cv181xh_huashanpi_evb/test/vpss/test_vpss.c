#include "board.h"
#include <aos/kernel.h>
#include <stdio.h>
#include <ulog/ulog.h>
#include <aos/cli.h>
#include "fatfs_vfs.h"
#include "vfs.h"
#include <cvi_base.h>
#include "cvi_vb.h"
#include "cvi_sys.h"
#include "cvi_buffer.h"
#include <pinctrl-mars.h>
#include "cvi_vpss.h"

void start_vpss(int32_t argc, char **argv)
{
	VPSS_GRP_ATTR_S stVpssGrpAttr;
	VPSS_CHN_ATTR_S stVpssChnAttr;
	VPSS_GRP VpssGrp = 0;
	VPSS_CHN VpssChn = 0;
	MMF_CHN_S stSrcChn;
	MMF_CHN_S stDestChn;
	CVI_S32 s32Ret;

	stVpssGrpAttr.stFrameRate.s32SrcFrameRate = -1;
	stVpssGrpAttr.stFrameRate.s32DstFrameRate = -1;
	stVpssGrpAttr.enPixelFormat = PIXEL_FORMAT_NV21;
	stVpssGrpAttr.u32MaxW = 1920;
	stVpssGrpAttr.u32MaxH = 1080;
	stVpssGrpAttr.u8VpssDev = 0;

	stVpssChnAttr.u32Width = 1280;
	stVpssChnAttr.u32Height = 720;
	stVpssChnAttr.enVideoFormat = VIDEO_FORMAT_LINEAR;
	stVpssChnAttr.enPixelFormat = PIXEL_FORMAT_NV21;
	stVpssChnAttr.stFrameRate.s32SrcFrameRate = -1;
	stVpssChnAttr.stFrameRate.s32DstFrameRate = -1;
	stVpssChnAttr.u32Depth = 0;
	stVpssChnAttr.bMirror = CVI_FALSE;
	stVpssChnAttr.bFlip = CVI_FALSE;
	stVpssChnAttr.stNormalize.bEnable = CVI_FALSE;
	stVpssChnAttr.stAspectRatio.enMode = ASPECT_RATIO_NONE;


	CVI_VPSS_CreateGrp(VpssGrp, &stVpssGrpAttr);
	CVI_VPSS_SetChnAttr(VpssGrp, VpssChn, &stVpssChnAttr);
	CVI_VPSS_EnableChn(VpssGrp, VpssChn);
	CVI_VPSS_StartGrp(VpssGrp);


	stSrcChn.enModId = CVI_ID_VI;
	stSrcChn.s32DevId = 0;
	stSrcChn.s32ChnId = 0;

	stDestChn.enModId = CVI_ID_VPSS;
	stDestChn.s32DevId = VpssGrp;
	stDestChn.s32ChnId = 0;

	s32Ret = CVI_SYS_Bind(&stSrcChn, &stDestChn);
	if(s32Ret == 0) {
		printf("*********VI bind VPSS Sucessful******** \n");
	} else {
		printf("*********VI bind VPSS Failed************\n");
	}
}
ALIOS_CLI_CMD_REGISTER(start_vpss, start_vpss, start vpss);


void GetFmtName(PIXEL_FORMAT_E enPixFmt, CVI_CHAR *szName)
{
	switch (enPixFmt)
	{
		case PIXEL_FORMAT_RGB_888:
			snprintf(szName, 10, "rgb");
			break;
		case PIXEL_FORMAT_BGR_888:
			snprintf(szName, 10, "bgr");
			break;
		case PIXEL_FORMAT_RGB_888_PLANAR:
			snprintf(szName, 10, "rgbm");
			break;
		case PIXEL_FORMAT_BGR_888_PLANAR:
			snprintf(szName, 10, "bgrm");
			break;
		case PIXEL_FORMAT_YUV_PLANAR_422:
			snprintf(szName, 10, "p422");
			break;
		case PIXEL_FORMAT_YUV_PLANAR_420:
			snprintf(szName, 10, "p420");
			break;
		case PIXEL_FORMAT_YUV_PLANAR_444:
			snprintf(szName, 10, "p444");
			break;
		case PIXEL_FORMAT_YUV_400:
			snprintf(szName, 10, "y");
			break;
		case PIXEL_FORMAT_HSV_888:
			snprintf(szName, 10, "hsv");
			break;
		case PIXEL_FORMAT_HSV_888_PLANAR:
			snprintf(szName, 10, "hsvm");
			break;
		case PIXEL_FORMAT_NV12:
			snprintf(szName, 10, "nv12");
			break;
		case PIXEL_FORMAT_NV21:
			snprintf(szName, 10, "nv21");
			break;
		case PIXEL_FORMAT_NV16:
			snprintf(szName, 10, "nv16");
			break;
		case PIXEL_FORMAT_NV61:
			snprintf(szName, 10, "nv61");
			break;
		case PIXEL_FORMAT_YUYV:
			snprintf(szName, 10, "yuyv");
			break;
		case PIXEL_FORMAT_UYVY:
			snprintf(szName, 10, "uyvy");
			break;
		case PIXEL_FORMAT_YVYU:
			snprintf(szName, 10, "yvyu");
			break;
		case PIXEL_FORMAT_VYUY:
			snprintf(szName, 10, "vyuy");
			break;

		default:
			snprintf(szName, 10, "unknown");
			break;
	}

}

CVI_VOID test_VpssDump(VPSS_GRP Grp, VPSS_CHN Chn, CVI_U32 u32FrameCnt)
{
	CVI_S32 s32MilliSec = 1000;
	CVI_U32 u32Cnt = u32FrameCnt;
	CVI_CHAR szFrameName[128], szPixFrm[10];
	CVI_BOOL bFlag = CVI_TRUE;
	int fd = 0;
	CVI_S32 i;
	CVI_U32 u32DataLen;
	VIDEO_FRAME_INFO_S stFrameInfo;


	/* get frame  */
	while (u32Cnt--) {
		if (CVI_VPSS_GetChnFrame(Grp, Chn, &stFrameInfo, s32MilliSec) != CVI_SUCCESS) {
			printf("Get frame fail \n");
			usleep(1000);
			continue;
		}
		if (bFlag) {
			/* make file name */
			GetFmtName(stFrameInfo.stVFrame.enPixelFormat, szPixFrm);
			snprintf(szFrameName, 128, SD_FATFS_MOUNTPOINT"/vpss_grp%d_chn%d_%dx%d_%s_%d.yuv", Grp, Chn,
					 stFrameInfo.stVFrame.u32Width, stFrameInfo.stVFrame.u32Height,
					 szPixFrm, u32FrameCnt);
			printf("Dump frame of vpss chn %d to file: \"%s\"\n", Chn, szFrameName);

			fd = aos_open(szFrameName, O_CREAT | O_RDWR | O_TRUNC);
			if (fd <= 0) {
				printf("aos_open dst file failed\n");
				CVI_VPSS_ReleaseChnFrame(Grp, Chn, &stFrameInfo);
				return;
			}

			bFlag = CVI_FALSE;
		}

		for (i = 0; i < 3; ++i) {
			u32DataLen = stFrameInfo.stVFrame.u32Stride[i] * stFrameInfo.stVFrame.u32Height;
			if (u32DataLen == 0)
				continue;
			if (i > 0 && ((stFrameInfo.stVFrame.enPixelFormat == PIXEL_FORMAT_YUV_PLANAR_420) ||
				(stFrameInfo.stVFrame.enPixelFormat == PIXEL_FORMAT_NV12) ||
				(stFrameInfo.stVFrame.enPixelFormat == PIXEL_FORMAT_NV21)))
				u32DataLen >>= 1;

			printf("plane(%d): paddr(%lx) vaddr(%p) stride(%d)\n",
				   i, stFrameInfo.stVFrame.u64PhyAddr[i],
				   stFrameInfo.stVFrame.pu8VirAddr[i],
				   stFrameInfo.stVFrame.u32Stride[i]);
			printf(" data_len(%d) plane_len(%d)\n",
					  u32DataLen, stFrameInfo.stVFrame.u32Length[i]);
			aos_write(fd, (CVI_U8 *)stFrameInfo.stVFrame.u64PhyAddr[i], u32DataLen);
		}

		if (CVI_VPSS_ReleaseChnFrame(Grp, Chn, &stFrameInfo) != CVI_SUCCESS)
			printf("CVI_VPSS_ReleaseChnFrame fail\n");
	}
	if (fd) {
		aos_sync(fd);
		aos_close(fd);
	}
}

void dump_vpss_frame(int32_t argc, char **argv)
{
	CVI_U32  VpssGrp = 0;
	CVI_U32  VpssChn = 0;
	CVI_S32  s32Cnt = 1;

	if (argc != 4) {
		printf("invailed param\n usage: %s [grp] [chn] [count]\n", argv[0]);
		return;
	}

	VpssGrp = atoi(argv[1]);
	VpssChn = atoi(argv[2]);
	s32Cnt = atoi(argv[3]);

	test_VpssDump(VpssGrp, VpssChn, s32Cnt);
}
ALIOS_CLI_CMD_REGISTER(dump_vpss_frame, dump_vpss_frame, dump vpss frame function);


