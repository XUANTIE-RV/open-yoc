/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: ldc_uapi.h
 * Description:
 */

#ifndef _U_LDC_UAPI_H_
#define _U_LDC_UAPI_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <cvi_comm_gdc.h>
#include "rtos_types.h"

enum cvi_ldc_op {
	CVI_LDC_OP_NONE,
	CVI_LDC_OP_XY_FLIP,
	CVI_LDC_OP_ROT_90,
	CVI_LDC_OP_ROT_270,
	CVI_LDC_OP_LDC,
	CVI_LDC_OP_MAX,
};

struct cvi_ldc_buffer {
	__u8 pixel_fmt; // 0: Y only, 1: NV21
	__u8 rot;
	__u16 bgcolor; // data outside start/end if used in operation

	__u16 src_width; // src width, including padding
	__u16 src_height; // src height, including padding

	__u32 src_y_base;
	__u32 src_c_base;
	__u32 dst_y_base;
	__u32 dst_c_base;

	__u32 map_base;
};

struct cvi_ldc_rot {
	__u64 handle;

	void *pUsageParam;
	void *vb_in;
	__u32 enPixFormat;
	__u64 mesh_addr;
	__u8 sync_io;
	void *cb;
	void *pcbParam;
	__u32 cbParamSize;
	__u32 enModId;
	__u32 enRotation;
};

struct gdc_handle_data {
	__u64 handle;
};

/*
 * stImgIn: Input picture
 * stImgOut: Output picture
 * au64privateData[4]: RW; Private data of task
 * reserved: RW; Debug information,state of current picture
 */
struct gdc_task_attr {
	__u64 handle;

	struct _VIDEO_FRAME_INFO_S stImgIn;
	struct _VIDEO_FRAME_INFO_S stImgOut;
	__u64 au64privateData[4];
	__u32 enRotation;
	__u64 reserved;
	union {
		FISHEYE_ATTR_S stFishEyeAttr;
		AFFINE_ATTR_S stAffineAttr;
		LDC_ATTR_S stLDCAttr;
	};

	CVI_U64 meshHandle;
	struct _LDC_BUF_WRAP_S stBufWrap;
	CVI_U32 bufWrapDepth;
	CVI_U64 bufWrapPhyAddr;
};

struct ldc_buf_wrap_cfg {
	__u64 handle;
	struct gdc_task_attr stTask;
	struct _LDC_BUF_WRAP_S stBufWrap;
};

#define CVI_LDC_BEGIN_JOB    1
#define CVI_LDC_END_JOB      2
#define CVI_LDC_CANCEL_JOB   3

#define CVI_LDC_ADD_ROT_TASK 4
#define CVI_LDC_ADD_LDC_TASK 5
#define CVI_LDC_SET_BUF_WRAP 6
#define CVI_LDC_GET_BUF_WRAP 7

#ifdef __cplusplus
}
#endif

#endif /* _U_LDC_UAPI_H_ */
