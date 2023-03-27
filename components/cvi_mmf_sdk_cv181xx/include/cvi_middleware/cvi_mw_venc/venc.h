#ifndef __VENC_H__
#define __VENC_H__

#include "stdint.h"
#include "queue.h"
#include <cvi_base_ctx.h>
#include "base_ctx.h"
#include "cvi_type.h"
#include "cvi_defines.h"
#include "cvi_comm_venc.h"
#include "enc_ctx.h"
#include "cvi_jpg_interface.h"
#include "cvi_pthread.h"


#ifndef MO_TBL_SIZE
#define MO_TBL_SIZE 256
#endif

#ifndef BIT
#define BIT(x) (1 << x)
#endif

// struct cvi_venc_vb_ctx {
// 	CVI_BOOL enable_bind_mode;
// 	CVI_BOOL currBindMode;
// 	PTHREAD_T thread;
// //	vb_jobs_t vb_jobs;
// 	CVI_BOOL pause;
// };

struct timespec64 {
	CVI_S64	tv_sec;			/* seconds */
	long	tv_nsec;		/* nanoseconds */
};

struct itimerspec64 {
	struct timespec64 it_interval;
	struct timespec64 it_value;
};

// struct crop_size {
// 	uint16_t  start_x;
// 	uint16_t  start_y;
// 	uint16_t  end_x;
// 	uint16_t  end_y;
// };

// struct cvi_buffer {
// 	uint64_t phy_addr[3];
// 	size_t length[3];
// 	uint32_t stride[3];
// 	SIZE_S size;
// 	uint64_t u64PTS;
// 	uint8_t dev_num;
// 	PIXEL_FORMAT_E enPixelFormat;
// 	uint32_t frm_num;
// 	struct crop_size frame_crop;

// 	int16_t s16OffsetTop;
// 	int16_t s16OffsetBottom;
// 	int16_t s16OffsetLeft;
// 	int16_t s16OffsetRight;

// 	uint8_t  motion_lv;
// 	uint8_t  motion_table[MO_TBL_SIZE];

// 	uint32_t flags;
// 	uint32_t sequence;
// 	struct timespec64 timestamp;
// };

struct hlist_head {
	struct hlist_node *first;
};

struct hlist_node {
	struct hlist_node *next, **pprev;
};

// struct vb_s {
// 	VB_POOL vb_pool;
// 	uint64_t phy_addr;
// 	void *vir_addr;
// 	int usr_cnt;
// 	struct cvi_buffer buf;
// 	uint32_t magic;
// 	uint64_t mod_ids;
// 	CVI_BOOL external;
// 	struct hlist_node node;
// };

typedef enum _VENC_CHN_STATE_ {
	VENC_CHN_STATE_NONE = 0,
	VENC_CHN_STATE_INIT,
	VENC_CHN_STATE_START_ENC,
	VENC_CHN_STATE_STOP_ENC,
} VENC_CHN_STATE;

typedef enum _VENC_SBM_STATE {
	VENC_SBM_STATE_IDLE = 0,
	VENC_SBM_STATE_FRM_RUN,
	VENC_SBM_STATE_FRM_SKIP,
	VENC_SBM_STATE_CHN_CLOSED,
	VENC_SBM_MAX_STATE
} VENC_SBM_STATE;

typedef struct _venc_frc {
	CVI_BOOL bFrcEnable;
	CVI_S32 srcFrameDur;
	CVI_S32 dstFrameDur;
	CVI_S32 srcTs;
	CVI_S32 dstTs;
} venc_frc;

typedef struct _venc_vfps {
	CVI_BOOL bVfpsEnable;
	CVI_S32 s32NumFrmsInOneSec;
	CVI_U64 u64prevSec;
	CVI_U64 u64StatTime;
} venc_vfps;

#define CVI_DEF_VFPFS_STAT_TIME 2
#define MAX_VENC_FRM_NUM 32

typedef struct _venc_chn_vars {
	CVI_U64 u64TimeOfSendFrame;
	CVI_U64 u64LastGetStreamTimeStamp;
	CVI_U64 u64LastSendFrameTimeStamp;
	CVI_U64 currPTS;
	CVI_U64 totalTime;
	CVI_S32 frameIdx;
	CVI_S32 s32RecvPicNum;
	CVI_S32 bind_event_fd;
	venc_frc frc;
	venc_vfps vfps;
	VENC_STREAM_S stStream;
	VENC_JPEG_PARAM_S stJpegParam;
	VENC_CHN_PARAM_S stChnParam;
	VENC_CHN_STATUS_S chnStatus;
	VENC_CU_PREDICTION_S cuPrediction;
	VENC_FRAME_PARAM_S frameParam;
	VENC_CHN_STATE chnState;
	USER_RC_INFO_S stUserRcInfo;
	VENC_SUPERFRAME_CFG_S stSuperFrmParam;
	ptread_sem_s sem_send;
	ptread_sem_s sem_release;
	CVI_BOOL bAttrChange;
	VENC_FRAMELOST_S frameLost;
	CVI_BOOL bHasVbPool;
	VENC_CHN_POOL_S vbpool;
	VB_BLK  vbBLK[VB_MAX_COMM_POOLS];
	cviBufInfo FrmArray[MAX_VENC_FRM_NUM];
	CVI_U32 FrmNum;
	CVI_U32 u32SendFrameCnt;
	CVI_U32 u32GetStreamCnt;
	CVI_S32 s32BindModeGetStreamRet;
	CVI_U32 u32FirstPixelFormat;
	CVI_BOOL bSendFirstFrm;
	CVI_U32 u32Stride[3];
	VIDEO_FRAME_INFO_S stFrameInfo;
	VENC_ROI_ATTR_S stRoiAttr[8];
	VCODEC_PERF_FPS_S stFPS;
	ptread_sem_s sem_vb;
} venc_chn_vars;

typedef struct _venc_chn_context {
	VENC_CHN VeChn;
	VENC_CHN_ATTR_S *pChnAttr;
	VENC_RC_PARAM_S rcParam;
	VENC_REF_PARAM_S refParam;
	VENC_FRAMELOST_S frameLost;
	VENC_H264_ENTROPY_S h264Entropy;
	VENC_H264_TRANS_S h264Trans;
	VENC_H265_TRANS_S h265Trans;
	union {
		VENC_H264_VUI_S h264Vui;
		VENC_H265_VUI_S h265Vui;
	};
	venc_enc_ctx encCtx;
	venc_chn_vars *pChnVars;
	struct cvi_venc_vb_ctx *pVbCtx;
	MUTEX_T chnMutex;
	MUTEX_T chnShmMutex;

	CVI_BOOL bSbSkipFrm;
	VENC_SBM_STATE sbm_state;
	CVI_U32 jpgFrmSkipCnt;

	PTHREAD_T pSBMSendFrameThread;
	VENC_SB_Setting stSbSetting;
	VIDEO_FRAME_INFO_S stVideoFrameInfo;

	struct vb_s vb;
	BOOL useVbFlag;
	CVI_U64 releaseVbAddr;

	CVI_BOOL bChnEnable;
} venc_chn_context;

typedef struct _CVI_VENC_MODPARAM_S {
	VENC_MOD_VENC_S stVencModParam;
	VENC_MOD_H264E_S stH264eModParam;
	VENC_MOD_H265E_S stH265eModParam;
	VENC_MOD_JPEGE_S stJpegeModParam;
	VENC_MOD_RC_S stRcModParam;
} CVI_VENC_PARAM_MOD_S;

typedef struct _venc_context {
	venc_chn_context * chn_handle[VENC_MAX_CHN_NUM];
	CVI_U32 chn_status[VENC_MAX_CHN_NUM];
	CVI_VENC_PARAM_MOD_S ModParam;
	pthread_t threadDataInput;
} venc_context;

typedef struct _venc_proc_info_t {
	VENC_CHN_ATTR_S chnAttr;
	VENC_CHN_PARAM_S stChnParam;
	VENC_CHN_STATUS_S chnStatus;
	VENC_RC_PARAM_S rcParam;
	VENC_REF_PARAM_S refParam;
	VENC_JPEG_PARAM_S stJpegParam;
	VENC_STREAM_S stStream;
	VIDEO_FRAME_S stFrame;
	VENC_FRAMELOST_S frameLost;
	VENC_ROI_ATTR_S stRoiAttr[8];
	VCODEC_PERF_FPS_S stFPS;
	VENC_SUPERFRAME_CFG_S stSuperFrmParam;
	uint16_t u16ChnNo;
	uint8_t u8ChnUsed;
} venc_proc_info_t;

typedef struct _proc_debug_config_t {
	uint32_t u32DbgMask;
	uint32_t u32StartFrmIdx;
	uint32_t u32EndFrmIdx;
	char cDumpPath[CVI_VENC_STR_LEN];
	uint32_t u32NoDataTimeout;
} proc_debug_config_t;
#endif

