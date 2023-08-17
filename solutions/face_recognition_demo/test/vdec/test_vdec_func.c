#include "board.h"
#include <aos/aos.h>
#include <aos/kernel.h>
#include <stdio.h>
#include <ulog/ulog.h>
#include <aos/cli.h>
#include <posix/timer.h>
#include <sys/time.h>
#include "cvi_sys.h"
#include "cvi_vb.h"
#include "cvi_vpss.h"
#include "cvi_buffer.h"
#include "cvi_vdec.h"
#include "sys/prctl.h"
#include "fatfs_vfs.h"
#include "vfs.h"

#define cvi_printf printf
#define DUMP_YUV 0
#define APP_CHECK_RET(actual, fmt, arg...)                                   \
	do {																		 \
		if ((actual) != 0) {													 \
			aos_cli_printf("[%d]:%s():ret=%d \n" fmt, __LINE__, __func__, actual, ## arg); \
			return;																\
		}																		 \
	} while (0)

#define APP_CHECK_RET2(actual, fmt, arg...)                                   \
	do {																		 \
		if ((actual) != 0) {													 \
			aos_cli_printf("[%d]:%s():ret=%d \n" fmt, __LINE__, __func__, actual, ## arg); \
			return actual;														 \
		}																		 \
	} while (0)

#define APP_CHECK_RET3(actual, fmt, arg...)                                   \
	do {																		 \
		if ((actual) != 0) {													 \
			aos_cli_printf("[%d]:%s():ret=%d \n" fmt, __LINE__, __func__, actual, ## arg); \
			return (void *)CVI_SUCCESS;														\
		}																		 \
	} while (0)

typedef struct _VDEC_ATTR {
	PAYLOAD_TYPE_E enType;
	PIXEL_FORMAT_E enPixelFormat;
	VIDEO_MODE_E   enMode;
	CVI_U32 u32Width;
	CVI_U32 u32Height;
	CVI_U32 u32FrameBufCnt;
	CVI_U32 u32DisplayFrameNum;
	CVI_U32 u32VdecChn;
	CVI_U32 vdecFrameNum;
	CVI_CHAR *inFile;
	CVI_CHAR *outFile;
} VDEC_ATTR;

VDEC_ATTR vdec_attr = {
	.u32VdecChn = 0,
	.enType = PT_H264,
	.enPixelFormat = PIXEL_FORMAT_YUV_PLANAR_420,
	.enMode = VIDEO_MODE_FRAME,
	.u32Width = 2304,
	.u32Height = 1536,
	.u32FrameBufCnt = 4,
	.u32DisplayFrameNum = 2,
	.vdecFrameNum = 5,
	.inFile = CONFIG_SD_FATFS_MOUNTPOINT"/enc-2k.264",
	.outFile = CONFIG_SD_FATFS_MOUNTPOINT"/out.yuv",
};

static pthread_t VdecSendStreamTask[VENC_MAX_CHN_NUM];

#if DUMP_YUV
static void get_chroma_size_shift_factor(PIXEL_FORMAT_E enPixelFormat, CVI_S32 *w_shift, CVI_S32 *h_shift)
{
	switch (enPixelFormat) {

	case PIXEL_FORMAT_NV12:
	case PIXEL_FORMAT_NV21:
		*w_shift = 0;
		*h_shift = 1;
		break;
	case PIXEL_FORMAT_YUV_PLANAR_420:
	default:
		*w_shift = 1;
		*h_shift = 1;
		break;
	}
}

static int write_yuv(int fd, VIDEO_FRAME_S stVFrame)
{
	CVI_S32 c_w_shift, c_h_shift;

	get_chroma_size_shift_factor(stVFrame.enPixelFormat, &c_w_shift, &c_h_shift);

	aos_write(fd, stVFrame.pu8VirAddr[0], stVFrame.u32Stride[0] * stVFrame.u32Height);
	soc_dcache_invalid_range((unsigned long)stVFrame.pu8VirAddr[0], stVFrame.u32Stride[0] * stVFrame.u32Height);

	if (stVFrame.pu8VirAddr[1]) {
		aos_write(fd, stVFrame.pu8VirAddr[1], stVFrame.u32Stride[1] * stVFrame.u32Height >> c_h_shift);
		soc_dcache_invalid_range((unsigned long)stVFrame.pu8VirAddr[1], stVFrame.u32Stride[1] * stVFrame.u32Height >> c_h_shift);
	}

	if (stVFrame.pu8VirAddr[2]) {
		aos_write(fd, stVFrame.pu8VirAddr[2], stVFrame.u32Stride[2] * stVFrame.u32Height >> c_h_shift);
		soc_dcache_invalid_range((unsigned long)stVFrame.pu8VirAddr[2], stVFrame.u32Stride[2] * stVFrame.u32Height >> c_h_shift);
	}

	return 0;
}
#endif

static CVI_S32 vdec_init(VDEC_ATTR vdecAttr)
{
	VDEC_CHN_ATTR_S stAttr = {0};
	VDEC_CHN_PARAM_S stParam = {0};
	VDEC_MOD_PARAM_S stModParam = {0};

	APP_CHECK_RET2(CVI_VDEC_GetModParam(&stModParam), "CVI_VDEC_GetModParam");
	stModParam.enVdecVBSource = VB_SOURCE_COMMON;
	APP_CHECK_RET2(CVI_VDEC_SetModParam(&stModParam), "CVI_VDEC_SetModParam");

	stAttr.enType = vdecAttr.enType;
	stAttr.enMode = vdecAttr.enMode;
	stAttr.u32PicWidth = vdecAttr.u32Width;
	stAttr.u32PicHeight = vdecAttr.u32Height;
	stAttr.u32StreamBufSize = ALIGN(vdecAttr.u32Width * vdecAttr.u32Height, 0x4000);
	stAttr.u32FrameBufSize = VDEC_GetPicBufferSize(
				vdecAttr.enType, vdecAttr.u32Width, vdecAttr.u32Height,
				vdecAttr.enPixelFormat, DATA_BITWIDTH_8, COMPRESS_MODE_NONE);
	stAttr.u32FrameBufCnt =vdecAttr.u32FrameBufCnt;
	APP_CHECK_RET2(CVI_VDEC_CreateChn(vdecAttr.u32VdecChn, &stAttr), "CVI_VDEC_CreateChn");

	APP_CHECK_RET2(CVI_VDEC_GetChnAttr(vdecAttr.u32VdecChn, &stAttr), "CVI_VDEC_GetChnAttr");
	APP_CHECK_RET2(CVI_VDEC_SetChnAttr(vdecAttr.u32VdecChn, &stAttr), "CVI_VDEC_SetChnAttr");

	APP_CHECK_RET2(CVI_VDEC_GetChnParam(vdecAttr.u32VdecChn, &stParam), "CVI_VDEC_GetChnParam");
	stParam.enPixelFormat = vdecAttr.enPixelFormat;
	APP_CHECK_RET2(CVI_VDEC_SetChnParam(vdecAttr.u32VdecChn, &stParam), "CVI_VDEC_SetChnParam");

	APP_CHECK_RET2(CVI_VDEC_StartRecvStream(vdecAttr.u32VdecChn), "CVI_VDEC_StartRecvStream");

	return CVI_SUCCESS;
}

static CVI_S32 vdec_deinit(VDEC_ATTR vdecAttr)
{
	APP_CHECK_RET2(CVI_VDEC_StopRecvStream(vdecAttr.u32VdecChn), "CVI_VDEC_StopRecvStream");
	APP_CHECK_RET2(CVI_VDEC_ResetChn(vdecAttr.u32VdecChn), "CVI_VDEC_ResetChn");
	APP_CHECK_RET2(CVI_VDEC_DestroyChn(vdecAttr.u32VdecChn), "CVI_VDEC_DestroyChn");

	return CVI_SUCCESS;
}

static CVI_VOID* vdec_streamproc(CVI_VOID *pArgs)
{
	CVI_S32 fdin = -1;
#if DUMP_YUV
	CVI_S32 fdout = -1;
#endif
	CVI_BOOL bFindStart, bFindEnd;
	CVI_BOOL bEndOfStream = CVI_FALSE;
	CVI_S32 s32UsedBytes = 0, s32ReadLen = 0;
	CVI_U32 u32Len, u32Start;
	CVI_S32 s32Ret, i;
	CVI_U32 u32MinBufSize = 700*1024;
	CVI_S32 s32MilliSec = 1000;
	CVI_U8 *pu8Buf = NULL;
	CVI_U64 u64PTS = 0;
	CVI_U32 decFrameCnt = 0;
	VDEC_STREAM_S stStream = {0};
	VIDEO_FRAME_INFO_S stVFrame = {0};
	VDEC_CHN_STATUS_S stStatus = {0};

	cvi_printf("vdec start chn:%d\n", vdec_attr.u32VdecChn);
	fdin = aos_open(vdec_attr.inFile, O_RDONLY);
	if (fdin <= 0) {
		CVI_VDEC_ERR("can't open file %s \n", vdec_attr.inFile);
		return (void *)CVI_SUCCESS;
	}
#if DUMP_YUV
	fdout = aos_open(vdec_attr.outFile, O_CREAT | O_TRUNC | O_RDWR);
	if (fdout < 0) {
		CVI_VDEC_ERR("can't open file %s\n", vdec_attr.outFile);
		aos_close(fdin);
		return (void *)CVI_SUCCESS;
	}
#endif
	pu8Buf = malloc(u32MinBufSize);
	if (pu8Buf == NULL) {
		CVI_VDEC_ERR("can't alloc in send stream thread!\n");
		aos_close(fdin);
#if DUMP_YUV
		aos_close(fdout);
#endif
		return (void *)CVI_SUCCESS;
	}

	prctl(PR_SET_NAME, "vdec_streamproc");

	while (decFrameCnt < vdec_attr.vdecFrameNum) {
		bEndOfStream = CVI_FALSE;
		bFindStart = CVI_FALSE;
		bFindEnd = CVI_FALSE;
		u32Start = 0;
		s32Ret = aos_lseek(fdin, s32UsedBytes, SEEK_SET);
		s32ReadLen = aos_read(fdin, pu8Buf, u32MinBufSize);
		if (s32ReadLen == 0) {
			break;
		}

		soc_dcache_clean_invalid_range((unsigned long)pu8Buf, s32ReadLen);

		if (vdec_attr.enType == PT_H264) {
			for (i = 0; i < s32ReadLen - 8; i++) {
				int tmp = pu8Buf[i + 3] & 0x1F;

				if (pu8Buf[i] == 0 && pu8Buf[i + 1] == 0 && pu8Buf[i + 2] == 1 &&
					(((tmp == 0x5 || tmp == 0x1) && ((pu8Buf[i + 4] & 0x80) == 0x80)) ||
					 (tmp == 20 && (pu8Buf[i + 7] & 0x80) == 0x80))) {
					bFindStart = CVI_TRUE;
					i += 8;
					break;
				}
			}

			for (; i < s32ReadLen - 8; i++) {
				int tmp = pu8Buf[i + 3] & 0x1F;

				if (pu8Buf[i] == 0 && pu8Buf[i + 1] == 0 && pu8Buf[i + 2] == 1 &&
					(tmp == 15 || tmp == 7 || tmp == 8 || tmp == 6 ||
					 ((tmp == 5 || tmp == 1) && ((pu8Buf[i + 4] & 0x80) == 0x80)) ||
					 (tmp == 20 && (pu8Buf[i + 7] & 0x80) == 0x80))) {
					bFindEnd = CVI_TRUE;
					break;
				}
			}

			if (i > 0)
				s32ReadLen = i;

			if (bFindStart == CVI_FALSE) {
				cvi_printf("chn %d can not find H264 start code!\n", vdec_attr.u32VdecChn);
				return (void *)CVI_SUCCESS;
			}

			if (bFindEnd == CVI_FALSE) {
				s32ReadLen = i + 8;
			}

		} else if (vdec_attr.enType == PT_JPEG) {
			for (i = 0; i < s32ReadLen - 1; i++) {
				if (pu8Buf[i] == 0xFF && pu8Buf[i + 1] == 0xD8) {
					u32Start = i;
					bFindStart = CVI_TRUE;
					i = i + 2;
					break;
				}
			}

			for (; i < s32ReadLen - 3; i++) {
				if ((pu8Buf[i] == 0xFF) && (pu8Buf[i + 1] & 0xF0) == 0xE0) {
					u32Len = (pu8Buf[i + 2] << 8) + pu8Buf[i + 3];
					i += 1 + u32Len;
				} else {
					break;
				}
			}

			for (; i < s32ReadLen - 1; i++) {
				if (pu8Buf[i] == 0xFF && pu8Buf[i + 1] == 0xD9) {
					bFindEnd = CVI_TRUE;
					break;
				}
			}
			s32ReadLen = i + 2;

			if (bFindStart == CVI_FALSE) {
				cvi_printf("can not find JPEG start code!!\n");
				return (void *)CVI_SUCCESS;
			}
		} else {
			CVI_VDEC_ERR("cannot support format\n");
		}
		
		stStream.u64PTS = u64PTS;
		stStream.pu8Addr = pu8Buf + u32Start;
		stStream.u32Len = s32ReadLen;
		stStream.bEndOfFrame = CVI_TRUE;
		stStream.bEndOfStream = bEndOfStream;
		stStream.bDisplay = 1;
SendAgain:
		s32Ret = CVI_VDEC_SendStream(vdec_attr.u32VdecChn, &stStream, s32MilliSec);
		if (s32Ret == CVI_SUCCESS) {
			bEndOfStream = CVI_FALSE;
			s32UsedBytes = s32UsedBytes + s32ReadLen + u32Start;
		} else if (s32Ret == CVI_ERR_VDEC_BUSY) {
			cvi_printf("timeout in vdec sendstream\n");
			goto SendAgain;
		} else {
			cvi_printf("CVI_VDEC_SendStream err, ret: 0x%x\n", s32Ret);
			break;
		}

		APP_CHECK_RET3(CVI_VDEC_QueryStatus(vdec_attr.u32VdecChn, &stStatus), "CVI_VDEC_QueryStatus");
		if (stStatus.u32LeftPics == 0)
			continue;

RETRY_GET_FRAME:
		s32Ret = CVI_VDEC_GetFrame(vdec_attr.u32VdecChn, &stVFrame, s32MilliSec);
		if (s32Ret == CVI_SUCCESS) {
			#if DUMP_YUV
			write_yuv(fdout, stVFrame.stVFrame);
			#endif
		} else if (s32Ret == CVI_ERR_VDEC_BUSY) {
			cvi_printf("vdec getframe timeout...retry\n");
			goto RETRY_GET_FRAME;
		} else {
			CVI_VDEC_ERR("vdec getframe Err, 0x%x\n", s32Ret);
			break;
		}

		APP_CHECK_RET3(CVI_VDEC_ReleaseFrame(vdec_attr.u32VdecChn, &stVFrame), "CVI_VDEC_ReleaseFrame");

		decFrameCnt++;
	}

	if ((vdec_attr.enType == PT_H264) || (vdec_attr.enType == PT_H265)) {
		/* send the flag of stream end */
		memset(&stStream, 0, sizeof(VDEC_STREAM_S));
		stStream.bEndOfStream = CVI_TRUE;
		APP_CHECK_RET3(CVI_VDEC_SendStream(vdec_attr.u32VdecChn, &stStream, -1), "CVI_VDEC_SendStream");
	}

	free(pu8Buf);
	aos_close(fdin);
#if DUMP_YUV
	aos_close(fdout);
#endif

	APP_CHECK_RET3(vdec_deinit(vdec_attr), "vdec_deinit");
	cvi_printf("vdec finish chn:%d\n", vdec_attr.u32VdecChn);
	return (void *)CVI_SUCCESS;
}

void _start_vdec(void)
{
	struct sched_param param;
	pthread_attr_t pthread_attr;

	APP_CHECK_RET(vdec_init(vdec_attr), "venc_init\n");

	param.sched_priority = 40;
	pthread_attr_init(&pthread_attr);
	pthread_attr_setschedpolicy(&pthread_attr, SCHED_RR);
	pthread_attr_setschedparam(&pthread_attr, &param);
	pthread_attr_setinheritsched(&pthread_attr, PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setstacksize(&pthread_attr, 8192);
	if (pthread_create(&VdecSendStreamTask[vdec_attr.u32VdecChn], &pthread_attr, vdec_streamproc, (void*)&vdec_attr)) {
		cvi_printf("[Chn %d] Send Stream pthread_create failed\n", vdec_attr.u32VdecChn);
		return ;
	}

	return ;
}

#if 1
void start_vdec(int32_t argc, char **argv)
{
	_start_vdec();
}
ALIOS_CLI_CMD_REGISTER(start_vdec, start_vdec, start vdec);
#endif

