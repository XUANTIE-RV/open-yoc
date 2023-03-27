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
#include "cvi_comm_gdc.h"
#include "cvi_buffer.h"
#include "cvi_gdc.h"
#include "cvi_vi.h"
#include "cvi_vpss.h"
#include "cvi_vo.h"
#include "cvi_comm_vi.h"
#include "cvi_comm_vpss.h"
#include "test_gdc.h"

CVI_S32 SAMPLE_COMM_SYS_Init(VB_CONFIG_S *pstVbConfig)
{
	CVI_S32 s32Ret = CVI_FAILURE;

	CVI_SYS_Exit();
	CVI_VB_Exit();

	if (pstVbConfig == NULL) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "input parameter is null, it is invaild!\n");
		return CVI_FAILURE;
	}

	s32Ret = CVI_VB_SetConfig(pstVbConfig);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "CVI_VB_SetConf failed!\n");
		return s32Ret;
	}

	s32Ret = CVI_VB_Init();
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "CVI_VB_Init failed!\n");
		return s32Ret;
	}

	s32Ret = CVI_SYS_Init();
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "CVI_SYS_Init failed!\n");
		CVI_VB_Exit();
		return s32Ret;
	}

	return CVI_SUCCESS;
}
CVI_VOID SAMPLE_COMM_SYS_Exit(void)
{
	CVI_SYS_Exit();
	CVI_VB_Exit();
}

#define GDC_FUNC_TEST_DEBUG 1
#define TILESIZE 64 // HW: data Tile Size
#define HW_MESH_SIZE 8
#define MESH_NUM_ATILE (TILESIZE / HW_MESH_SIZE) // how many mesh in A TILE

typedef struct COORD2D_INT_HW {
	CVI_U8 xcor[3]; // s13.10, 24bit
} __attribute__((packed)) COORD2D_INT_HW;

static void mesh_gen_get_1st_size(SIZE_S in_size, CVI_U32 *mesh_1st_size)
{
	if (!mesh_1st_size)
		return;

	CVI_U32 ori_src_width = in_size.u32Width;
	CVI_U32 ori_src_height = in_size.u32Height;

	// In LDC Processing, width & height  aligned to TILESIZE **
	CVI_U32 src_width_s1 =
		((ori_src_width + TILESIZE - 1) / TILESIZE) * TILESIZE;
	CVI_U32 src_height_s1 =
		((ori_src_height + TILESIZE - 1) / TILESIZE) * TILESIZE;

	// modify frame size
	CVI_U32 dst_height_s1 = src_height_s1;
	CVI_U32 dst_width_s1 = src_width_s1;
	CVI_U32 num_tilex_s1 = dst_width_s1 / TILESIZE;
	CVI_U32 num_tiley_s1 = dst_height_s1 / TILESIZE;

	*mesh_1st_size = sizeof(struct COORD2D_INT_HW) * MESH_NUM_ATILE *
			 MESH_NUM_ATILE * num_tilex_s1 *
			 num_tiley_s1 * 4; // 4 = 4 knots in a mesh
}

static CVI_S32 test_gdc_config_video_frame(SIZE_S size, PIXEL_FORMAT_E PixelFmt, VIDEO_FRAME_INFO_S *pstVideoFrame)
{
	VB_CAL_CONFIG_S stVbCalConfig;
	VB_BLK blk;

	COMMON_GetPicBufferConfig(size.u32Width, size.u32Height
		, PixelFmt, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, DEFAULT_ALIGN, &stVbCalConfig);
	printf("stVbCalConfig.u32VBSize:%d\n", stVbCalConfig.u32VBSize);
	blk = CVI_VB_GetBlock(VB_INVALID_POOLID, stVbCalConfig.u32VBSize);
	if (blk == VB_INVALID_HANDLE) {
		printf("CVI_VB_GetBlock fail");
		return CVI_FAILURE;
	}

	pstVideoFrame->u32PoolId = CVI_VB_Handle2PoolId(blk);
	pstVideoFrame->stVFrame.enPixelFormat = PixelFmt;
	pstVideoFrame->stVFrame.u32Width = size.u32Width;
	pstVideoFrame->stVFrame.u32Height = size.u32Height;

	for (int i = 0; i < stVbCalConfig.plane_num; i++) {
		pstVideoFrame->stVFrame.u32Stride[i] = (i == 0) ?
			stVbCalConfig.u32MainStride : stVbCalConfig.u32CStride;

		pstVideoFrame->stVFrame.u32Length[i] = (i == 0) ?
			stVbCalConfig.u32MainYSize : stVbCalConfig.u32MainCSize;

		pstVideoFrame->stVFrame.u64PhyAddr[i] = (i == 0) ?
			CVI_VB_Handle2PhysAddr(blk) :
			pstVideoFrame->stVFrame.u64PhyAddr[i-1]
			+ ALIGN(pstVideoFrame->stVFrame.u32Length[i-1], stVbCalConfig.u16AddrAlign);
	}

#if GDC_FUNC_TEST_DEBUG
	printf("phy addr(%#lx, %#lx, %#lx)\n", pstVideoFrame->stVFrame.u64PhyAddr[0]
		, pstVideoFrame->stVFrame.u64PhyAddr[1], pstVideoFrame->stVFrame.u64PhyAddr[2]);

	printf("u32Stride(%d, %d, %d)\n", pstVideoFrame->stVFrame.u32Stride[0]
		, pstVideoFrame->stVFrame.u32Stride[1], pstVideoFrame->stVFrame.u32Stride[2]);

	printf("u32Length(%d, %d, %d)\n", pstVideoFrame->stVFrame.u32Length[0]
		, pstVideoFrame->stVFrame.u32Length[1], pstVideoFrame->stVFrame.u32Length[2]);
#endif

	return CVI_SUCCESS;
}

static CVI_S32 test_gdc_load_frame(CVI_CHAR *name, SIZE_S size_in
	, PIXEL_FORMAT_E PixelFmt, VIDEO_FRAME_INFO_S *pstDestVideoFrame)
{
	int fd;
	size_t len;
	CVI_S32 Ret = CVI_SUCCESS;
	VB_CAL_CONFIG_S stVbCalConfig;

	COMMON_GetPicBufferConfig(size_in.u32Width, size_in.u32Height
			, PixelFmt, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, DEFAULT_ALIGN, &stVbCalConfig);

	fd = aos_open(name, O_RDONLY);
	if (fd < 0) {
		printf("open file fail\n");
		return CVI_FAILURE;
	}

	for (int i = 0; i < stVbCalConfig.plane_num; ++i) {
		pstDestVideoFrame->stVFrame.pu8VirAddr[i] = (CVI_U8 *)pstDestVideoFrame->stVFrame.u64PhyAddr[i];
			//= CVI_SYS_Mmap(pstDestVideoFrame->stVFrame.u64PhyAddr[i]
			//, pstDestVideoFrame->stVFrame.u32Length[i]);

#if GDC_FUNC_TEST_DEBUG
		printf("phy addr(%#lx, vir addr(%p), len:%d\n", pstDestVideoFrame->stVFrame.u64PhyAddr[i]
			, pstDestVideoFrame->stVFrame.pu8VirAddr[i], pstDestVideoFrame->stVFrame.u32Length[i]);
#endif
		if (i == 0) {
			len = aos_read(fd, (void *)pstDestVideoFrame->stVFrame.pu8VirAddr[i]
				, stVbCalConfig.u32MainYSize);
			if (len != stVbCalConfig.u32MainYSize) {
				printf("[%d]read file fail\n", i);
				aos_close(fd);
				return CVI_FAILURE;
			}
		} else {
			len = aos_read(fd, (void *)pstDestVideoFrame->stVFrame.pu8VirAddr[i]
				, stVbCalConfig.u32MainCSize);
			if (len != stVbCalConfig.u32MainCSize) {
				printf("[%d]read file fail\n", i);
				aos_close(fd);
				return CVI_FAILURE;
			}
		}

		//Ret = CVI_SYS_Munmap(pstDestVideoFrame->stVFrame.pu8VirAddr[i]
		//	, pstDestVideoFrame->stVFrame.u32Length[i]);
	}
	aos_sync(fd);
	Ret = aos_close(fd);

	return Ret;
}
CVI_S32 SAMPLE_COMM_FRAME_SaveToFile(const CVI_CHAR *filename, VIDEO_FRAME_INFO_S *pstVideoFrame)
{
	int fd;
	CVI_U32 u32len, u32DataLen;

	fd = aos_open(filename, O_WRONLY | O_CREAT | O_SYNC);
	if (fd < 0) {
		printf("open file fail\n");
		return CVI_FAILURE;
	}

	for (int i = 0; i < 3; ++i) {
		u32DataLen = pstVideoFrame->stVFrame.u32Stride[i] * pstVideoFrame->stVFrame.u32Height;
		if (u32DataLen == 0)
			continue;
		if (i > 0 && ((pstVideoFrame->stVFrame.enPixelFormat == PIXEL_FORMAT_YUV_PLANAR_420) ||
			(pstVideoFrame->stVFrame.enPixelFormat == PIXEL_FORMAT_NV12) ||
			(pstVideoFrame->stVFrame.enPixelFormat == PIXEL_FORMAT_NV21)))
			u32DataLen >>= 1;

		pstVideoFrame->stVFrame.pu8VirAddr[i] = (CVI_U8 *)pstVideoFrame->stVFrame.u64PhyAddr[i];
			//= CVI_SYS_Mmap(pstVideoFrame->stVFrame.u64PhyAddr[i], pstVideoFrame->stVFrame.u32Length[i]);

		printf("plane(%d): paddr(%#lx) vaddr(%p) stride(%d)\n",
			   i, pstVideoFrame->stVFrame.u64PhyAddr[i],
			   pstVideoFrame->stVFrame.pu8VirAddr[i],
			   pstVideoFrame->stVFrame.u32Stride[i]);
		printf(" data_len(%d) plane_len(%d)\n",
			      u32DataLen, pstVideoFrame->stVFrame.u32Length[i]);
		u32len = aos_write(fd, (void *)pstVideoFrame->stVFrame.pu8VirAddr[i], u32DataLen);
		if (u32len != u32DataLen) {
			CVI_TRACE_LOG(CVI_DBG_ERR, "fwrite data(%d) error\n", i);
			return CVI_FAILURE;
		}
		//CVI_SYS_Munmap(pstVideoFrame->stVFrame.pu8VirAddr[i], pstVideoFrame->stVFrame.u32Length[i]);
	}
	aos_sync(fd);
	aos_close(fd);
	return CVI_SUCCESS;
}

void test_gdc_begain_job(int32_t argc, char **argv)
{
	CVI_S32 s32Ret = CVI_SUCCESS;
	VB_CONFIG_S stVbConf;
	GDC_HANDLE hHandle;
	GDC_TASK_ATTR_S stTask;
	ROTATION_E enRotation = ROTATION_0;
	PIXEL_FORMAT_E pixelFormat = PIXEL_FORMAT_NV21;
	VB_BLK blk = VB_INVALID_HANDLE, vbOutBlk = VB_INVALID_HANDLE;
	SIZE_S stSize, stSizeOut;
	SIZE_S stSizeInCfg;
	CVI_U32 u32BlkSize, u32BlkSizeRot;
	CVI_CHAR pszInFileName[64] = SD_FATFS_MOUNTPOINT"/1920_1080_nv21.yuv";
	CVI_CHAR pszOutFileName[64] = SD_FATFS_MOUNTPOINT"/1920_1080_ldc_nv21.yuv";
	CVI_U64 u64OutPhyAddr;
	CVI_U8 *pu8OutVirtAddr;
	VB_CAL_CONFIG_S stVbOutCalConfig;
	LDC_ATTR_S stLDCAttr = {CVI_TRUE, 50, 50, 50, 0, 0, 200};
	CVI_BOOL LDCEnable = CVI_TRUE;
	CVI_U64 u64PhyAddr;
	CVI_VOID *pVirAddr;

	stSize.u32Width = 1920;
	stSize.u32Height = 1080;
	if (enRotation == ROTATION_90 || enRotation == ROTATION_270 || enRotation == ROTATION_XY_FLIP) {
		stSizeOut.u32Width = ALIGN(stSize.u32Height, DEFAULT_ALIGN);
		stSizeOut.u32Height = ALIGN(stSize.u32Width, DEFAULT_ALIGN);
	} else {// flat or ldc need align
		stSizeOut.u32Width = ALIGN(stSize.u32Width, DEFAULT_ALIGN);
		stSizeOut.u32Height = ALIGN(stSize.u32Height, DEFAULT_ALIGN);
	}

	memset(&stVbConf, 0, sizeof(VB_CONFIG_S));
	stVbConf.u32MaxPoolCnt = 1;

	u32BlkSize = COMMON_GetPicBufferSize(stSizeOut.u32Width, stSizeOut.u32Height, PIXEL_FORMAT_NV21,
		DATA_BITWIDTH_8, COMPRESS_MODE_NONE, DEFAULT_ALIGN);
	u32BlkSizeRot = COMMON_GetPicBufferSize(stSizeOut.u32Width, stSizeOut.u32Height, PIXEL_FORMAT_NV21,
		DATA_BITWIDTH_8, COMPRESS_MODE_NONE, DEFAULT_ALIGN);
	u32BlkSize = u32BlkSize > u32BlkSizeRot ? u32BlkSize : u32BlkSizeRot;
	stVbConf.astCommPool[0].u32BlkSize	= u32BlkSize;
	stVbConf.astCommPool[0].u32BlkCnt	= 2;
	printf("u32BlkSize:%d\n", u32BlkSize);
	SAMPLE_COMM_SYS_Init(&stVbConf);

	memset(&stTask, 0, sizeof(stTask));

	//config in img
	stSizeInCfg.u32Width = ALIGN(stSize.u32Width, DEFAULT_ALIGN);
	stSizeInCfg.u32Height = ALIGN(stSize.u32Height, DEFAULT_ALIGN);
	s32Ret = test_gdc_config_video_frame(stSizeInCfg, pixelFormat, &stTask.stImgIn);
	if (s32Ret != CVI_SUCCESS) {
		printf("test_gdc_config_video_frame failed, s32Ret:0x%x\n", s32Ret);
		goto EXIT;
	}
	s32Ret = test_gdc_load_frame(pszInFileName, stSize, pixelFormat, &stTask.stImgIn);
	if (s32Ret != CVI_SUCCESS) {
		printf("test_gdc_load_frame failed, s32Ret:0x%x\n", s32Ret);
		goto EXIT;
	}
	blk = CVI_VB_PhysAddr2Handle(stTask.stImgIn.stVFrame.u64PhyAddr[0]);
	printf("CVI_GDC_BeginJob\n");

	s32Ret = CVI_GDC_BeginJob(&hHandle);
	if (s32Ret != CVI_SUCCESS) {
		printf("CVI_GDC_BeginJob failed, s32Ret:0x%x", s32Ret);
		goto EXIT1;
	}

	vbOutBlk = CVI_VB_GetBlock(VB_INVALID_POOLID, u32BlkSize);
	if (VB_INVALID_HANDLE == vbOutBlk) {
		printf("CVI_VB_GetBlock(size:%d) fail\n", u32BlkSize);
		goto EXIT1;
	}

	u64OutPhyAddr = CVI_VB_Handle2PhysAddr(vbOutBlk);
	if (0 == u64OutPhyAddr) {
		printf("CVI_VB_Handle2PhyAddr fail, u32OutPhyAddr:0x%lx\n", u64OutPhyAddr);
		goto EXIT2;
	}

	pu8OutVirtAddr = (CVI_U8 *)u64OutPhyAddr;
	if (NULL == pu8OutVirtAddr) {
		printf("Info:CVI_SYS_Mmap fail, u8OutVirtAddr:0x%x\n", (CVI_U32)(uintptr_t)pu8OutVirtAddr);
		goto EXIT2;
	}

	COMMON_GetPicBufferConfig(stSizeOut.u32Width, stSizeOut.u32Height, pixelFormat, DATA_BITWIDTH_8
		, COMPRESS_MODE_NONE, DEFAULT_ALIGN, &stVbOutCalConfig);

	memcpy(&stTask.stImgOut, &stTask.stImgIn, sizeof(VIDEO_FRAME_INFO_S));

	stTask.stImgOut.u32PoolId		= CVI_VB_Handle2PoolId(vbOutBlk);
	stTask.stImgOut.stVFrame.u64PhyAddr[0]	= u64OutPhyAddr;
	stTask.stImgOut.stVFrame.u64PhyAddr[1]	= u64OutPhyAddr
						+ stVbOutCalConfig.u32MainStride * stSizeOut.u32Height;
	stTask.stImgOut.stVFrame.u64PhyAddr[2]	= 0;
	stTask.stImgOut.stVFrame.pu8VirAddr[0]	= pu8OutVirtAddr;
	stTask.stImgOut.stVFrame.pu8VirAddr[1]	= pu8OutVirtAddr
						+ stVbOutCalConfig.u32MainStride * stSizeOut.u32Height;
	stTask.stImgOut.stVFrame.pu8VirAddr[2]	= 0;
	stTask.stImgOut.stVFrame.u32Stride[0]	= stVbOutCalConfig.u32MainStride;
	stTask.stImgOut.stVFrame.u32Stride[1]	= stVbOutCalConfig.u32CStride;
	stTask.stImgOut.stVFrame.u32Stride[2]	= 0;
	stTask.stImgOut.stVFrame.u32Width	= stSizeOut.u32Width;
	stTask.stImgOut.stVFrame.u32Height	= stSizeOut.u32Height;

	if (LDCEnable) {
		stTask.stImgOut.stVFrame.u32Width = ALIGN(stSize.u32Width, DEFAULT_ALIGN);
		stTask.stImgOut.stVFrame.u32Height = ALIGN(stSize.u32Height, DEFAULT_ALIGN);
		s32Ret = CVI_GDC_GenLDCMesh(stTask.stImgOut.stVFrame.u32Width, stTask.stImgOut.stVFrame.u32Height
			, &stLDCAttr, "ldc_user", &u64PhyAddr, &pVirAddr);
		if (s32Ret != CVI_SUCCESS) {
			printf("CVI_GDC_GenLDCMesh failed, s32Ret:0x%x", s32Ret);
			CVI_GDC_CancelJob(hHandle);
			goto EXIT1;
		}

		GDC_TASK_ATTR_S stTaskArr[2];
		SIZE_S size_out[2];
		ROTATION_E enRotationOut[2];
		CVI_U32 mesh_1st_size;

		// Rotate 90/270 for 1st job
		size_out[0].u32Width = ALIGN(stSize.u32Height, DEFAULT_ALIGN);
		size_out[0].u32Height = ALIGN(stSize.u32Width, DEFAULT_ALIGN);

		if (enRotation == ROTATION_0 || enRotation == ROTATION_180) {
			size_out[1].u32Width = ALIGN(stSize.u32Width, DEFAULT_ALIGN);
			size_out[1].u32Height = ALIGN(stSize.u32Height, DEFAULT_ALIGN);
		} else {
			size_out[1].u32Width = ALIGN(stSize.u32Height, DEFAULT_ALIGN);
			size_out[1].u32Height = ALIGN(stSize.u32Width, DEFAULT_ALIGN);
		}
		stTask.stImgOut.stVFrame.u32Width = size_out[0].u32Width;
		stTask.stImgOut.stVFrame.u32Height = size_out[0].u32Height;
		switch (enRotation) {
		default:
		case ROTATION_0:
			enRotationOut[0] = ROTATION_90;
			enRotationOut[1] = ROTATION_270;
			break;
		case ROTATION_90:
			enRotationOut[0] = ROTATION_90;
			enRotationOut[1] = ROTATION_0;
			break;
		case ROTATION_180:
			enRotationOut[0] = ROTATION_90;
			enRotationOut[1] = ROTATION_90;
			break;
		case ROTATION_270:
			enRotationOut[0] = ROTATION_270;
			enRotationOut[1] = ROTATION_0;
			break;
		}
		memcpy(&stTaskArr[0], &stTask, sizeof(stTask));
		stTaskArr[0].reserved = 0;//magic id
		stTaskArr[0].au64privateData[0] = u64PhyAddr;
		stTaskArr[0].au64privateData[1] = (uintptr_t)pVirAddr;
		stTaskArr[0].au64privateData[2] = CVI_FALSE;
		s32Ret = CVI_GDC_AddLDCTask(hHandle, &stTaskArr[0], &stLDCAttr, enRotationOut[0]);
		if (s32Ret != CVI_SUCCESS) {
			printf("CVI_GDC_AddLDCTask 1st failed, s32Ret:0x%x", s32Ret);
			CVI_GDC_CancelJob(hHandle);
			goto EXIT1;
		}

		mesh_gen_get_1st_size(size_out[0], &mesh_1st_size);
		memcpy(&stTaskArr[1].stImgIn, &stTask.stImgOut, sizeof(stTask.stImgOut));
		memcpy(&stTaskArr[1].stImgOut, &stTask.stImgIn, sizeof(stTask.stImgIn));
		stTaskArr[1].stImgOut.stVFrame.u32Width = size_out[1].u32Width;
		stTaskArr[1].stImgOut.stVFrame.u32Height = size_out[1].u32Height;
		stTaskArr[1].reserved = 0;//magic id
		stTaskArr[1].au64privateData[0] = u64PhyAddr + mesh_1st_size;
		stTaskArr[1].au64privateData[1] = (uintptr_t)pVirAddr;//save orig mesh addr
		stTaskArr[1].au64privateData[2] = CVI_TRUE;
		s32Ret = CVI_GDC_AddLDCTask(hHandle, &stTaskArr[1], &stLDCAttr, enRotationOut[1]);
		if (s32Ret != CVI_SUCCESS) {
			printf("CVI_GDC_AddLDCTask 2nd failed, s32Ret:0x%x", s32Ret);
			CVI_GDC_CancelJob(hHandle);
			goto EXIT1;
		}
		memcpy(&stTask, &stTaskArr[1], sizeof(GDC_TASK_ATTR_S));
	} else {
		s32Ret = CVI_GDC_AddRotationTask(hHandle, &stTask, enRotation);
		if (s32Ret != CVI_SUCCESS) {
			printf("CVI_GDC_AddRotationTask failed, s32Ret:0x%x", s32Ret);
			CVI_GDC_CancelJob(hHandle);
			goto EXIT1;
		}
	}
	printf("CVI_GDC_EndJob\n");

	s32Ret = CVI_GDC_EndJob(hHandle);
	if (s32Ret != CVI_SUCCESS) {
		printf("CVI_GDC_EndJob failed, s32Ret:0x%x\n", s32Ret);
		CVI_GDC_CancelJob(hHandle);
		goto EXIT1;
	}
	printf("SAMPLE_COMM_FRAME_SaveToFile\n");

	s32Ret = SAMPLE_COMM_FRAME_SaveToFile(pszOutFileName, &stTask.stImgOut);
	if (s32Ret != CVI_SUCCESS) {
		printf("SAMPLE_COMM_FRAME_SaveToFile failed, s32Ret:0x%x\n", s32Ret);
		goto EXIT1;
	}

EXIT2:
	if (vbOutBlk != VB_INVALID_HANDLE)
		CVI_VB_ReleaseBlock(vbOutBlk);
EXIT1:
	if (blk != VB_INVALID_HANDLE)
		CVI_VB_ReleaseBlock(blk);
EXIT:
	SAMPLE_COMM_SYS_Exit();
}

void test_gdc_enable_rot(int32_t argc, char **argv)
{
	CVI_S32 ret;
	VI_PIPE ViPipe = 0;
	VI_CHN ViChn = 0;
	MOD_ID_E enModId;
	ROTATION_E rot;

	if (argc == 3) {
		enModId = atoi(argv[1]);
		rot = atoi(argv[2]);
	}
	if (argc != 3 || enModId < 0 || enModId > 2
		|| rot < 0 || rot > 4 || rot == 2) {
		printf("invailed param\n");
		printf("usage: %s [mod:0(VI)/1(VPSS)2(VO)][rot:0(rot0)/1(rot90)3(rot270)4(rot_xy_flip)]\n"
			, argv[0]);
		return;
	}
	if (enModId == 0)
		enModId = CVI_ID_VI;
	else if (enModId == 1)
		enModId = CVI_ID_VPSS;
	else if (enModId == 2)
		enModId = CVI_ID_VO;

	if (enModId == CVI_ID_VI) {
		ret = CVI_VI_SetChnRotation(ViPipe, ViChn, (ROTATION_E)rot);
	} else if (enModId == CVI_ID_VPSS) {
		ret = CVI_VPSS_SetChnRotation(0, 0, (ROTATION_E)rot);
	} else if (enModId == CVI_ID_VO) {
		ret = CVI_VO_SetChnRotation(0, 0, (ROTATION_E)rot);
	} else {
		ret = -1;
	}
	if (ret) {
		printf("mod:%d SetChnRotation failed, ret:0x%x\n", enModId, ret);
		return;
	}
	printf("mod:%d SetChnRotation(%d) OK\n", enModId, rot);
}

void test_gdc_enable_ldc(int32_t argc, char **argv)
{
	CVI_S32 ret;
	VI_PIPE ViPipe = 0;
	VI_CHN ViChn = 0;
	LDC_ATTR_S stLDCAttr = {CVI_TRUE, 50, 50, 50, 0, 0, 200};
	VI_LDC_ATTR_S stViLDCAttr;
	VPSS_LDC_ATTR_S stVpssLDCAttr;
	LOG_LEVEL_CONF_S stConf;
	MOD_ID_E enModId;

	if (argc == 2)
		enModId = atoi(argv[1]);
	if (argc != 2 || enModId < 0 || enModId > 1) {
		printf("invailed param\n");
		printf("usage: %s [mod:0(VI)/1(VPSS)]\n", argv[0]);
		return;
	}
	if (enModId == 0)
		enModId = CVI_ID_VI;
	else if (enModId == 1)
		enModId = CVI_ID_VPSS;

	stConf.enModId = CVI_ID_GDC;
	stConf.s32Level = CVI_DBG_DEBUG;
	CVI_LOG_SetLevelConf(&stConf);

	if (enModId == CVI_ID_VI) {
		stViLDCAttr.bEnable = CVI_TRUE;
		memcpy(&stViLDCAttr.stAttr, &stLDCAttr, sizeof(stLDCAttr));
		ret = CVI_VI_SetChnLDCAttr(ViPipe, ViChn, &stViLDCAttr);
	} else if (enModId == CVI_ID_VPSS) {
		stVpssLDCAttr.bEnable = CVI_TRUE;
		memcpy(&stVpssLDCAttr.stAttr, &stLDCAttr, sizeof(stLDCAttr));
		ret = CVI_VPSS_SetChnLDCAttr(0, 0, &stVpssLDCAttr);
	} else {
		ret = -1;
	}

	if (ret) {
		printf("mod:%d SetChnLDCAttr failed, ret:0x%x\n", enModId, ret);
		return;
	}
	printf("mod:%d SetChnLDCAttr OK\n", enModId);
}

void gdc_gen_mesh(int32_t argc, char **argv)
{
	int ret;
	CVI_U64 u64PhyAddr;
	CVI_VOID *pVirAddr;
	LDC_ATTR_S stLDCAttr = {CVI_TRUE, 50, 50, 50, 0, 0, 200};

	ret = CVI_GDC_GenLDCMesh(320, 240, &stLDCAttr, "ldc_user", &u64PhyAddr, &pVirAddr);
	if (ret)
		printf("gdc_gen_mesh fail\n");
	else
		printf("gdc_gen_mesh ok\n");
}


ALIOS_CLI_CMD_REGISTER(test_gdc_begain_job, test_gdc_begain_job, test_gdc_begain_job)
ALIOS_CLI_CMD_REGISTER(test_gdc_enable_rot, test_gdc_enable_rot, test_gdc_enable_rot)
ALIOS_CLI_CMD_REGISTER(test_gdc_enable_ldc, test_gdc_enable_ldc, test_gdc_enable_ldc)
ALIOS_CLI_CMD_REGISTER(gdc_gen_mesh, gdc_gen_mesh, gdc_gen_mesh)
