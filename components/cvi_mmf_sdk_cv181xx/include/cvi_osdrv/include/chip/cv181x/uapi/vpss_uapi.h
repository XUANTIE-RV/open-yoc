/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: vpss_uapi.h
 * Description:
 */

#ifndef _U_VPSS_UAPI_H_
#define _U_VPSS_UAPI_H_

#include <cvi_comm_vpss.h>
#include <cvi_comm_sys.h>
#include "rtos_types.h"

#ifdef __cplusplus
extern "C" {
#endif

struct vpss_crt_grp_cfg {
	VPSS_GRP VpssGrp;
	VPSS_GRP_ATTR_S stGrpAttr;
};

struct vpss_str_grp_cfg {
	VPSS_GRP VpssGrp;
};

struct vpss_grp_attr {
	VPSS_GRP VpssGrp;
	VPSS_GRP_ATTR_S stGrpAttr;
};

struct vpss_grp_crop_cfg {
	VPSS_GRP VpssGrp;
	VPSS_CROP_INFO_S stCropInfo;
};

struct vpss_grp_frame_cfg {
	VPSS_GRP VpssGrp;
	VIDEO_FRAME_INFO_S stVideoFrame;
};

struct vpss_snd_frm_cfg {
	__u8 VpssGrp;
	VIDEO_FRAME_INFO_S stVideoFrame;
	__s32 s32MilliSec;
};

struct vpss_chn_frm_cfg {
	VPSS_GRP VpssGrp;
	VPSS_CHN VpssChn;
	VIDEO_FRAME_INFO_S stVideoFrame;
	CVI_S32 s32MilliSec;
};

struct vpss_chn_attr {
	VPSS_GRP VpssGrp;
	VPSS_CHN VpssChn;
	VPSS_CHN_ATTR_S stChnAttr;
};

struct vpss_en_chn_cfg {
	CVI_U8 enable;
	VPSS_GRP VpssGrp;
	VPSS_CHN VpssChn;
};

struct vpss_chn_crop_cfg {
	VPSS_GRP VpssGrp;
	VPSS_CHN VpssChn;
	VPSS_CROP_INFO_S stCropInfo;
};

struct vpss_chn_rot_cfg {
	VPSS_GRP VpssGrp;
	VPSS_CHN VpssChn;
	ROTATION_E enRotation;
};

/* Prevent mw build error */
struct cvi_vpss_chn_rot_cfg {
	VPSS_GRP VpssGrp;
	VPSS_CHN VpssChn;
	ROTATION_E enRotation;
};

struct vpss_chn_ldc_cfg {
	VPSS_GRP VpssGrp;
	VPSS_CHN VpssChn;
	ROTATION_E enRotation;
	VPSS_LDC_ATTR_S stLDCAttr;
	CVI_U64 meshHandle;
};

/* Prevent mw build error */
struct cvi_vpss_chn_ldc_cfg {
	VPSS_GRP VpssGrp;
	VPSS_CHN VpssChn;
	ROTATION_E enRotation;
	VPSS_LDC_ATTR_S stLDCAttr;
	CVI_U64 meshHandle;
};

struct vpss_chn_align_cfg {
	VPSS_GRP VpssGrp;
	VPSS_CHN VpssChn;
	CVI_U32 u32Align;
};

struct vpss_chn_yratio_cfg {
	VPSS_GRP VpssGrp;
	VPSS_CHN VpssChn;
	CVI_U32 YRatio;
};

struct vpss_chn_coef_level_cfg {
	VPSS_GRP VpssGrp;
	VPSS_CHN VpssChn;
	VPSS_SCALE_COEF_E enCoef;
};

struct vpss_chn_buf_wrap_cfg {
	VPSS_GRP VpssGrp;
	VPSS_CHN VpssChn;
	VPSS_CHN_BUF_WRAP_S stBufWrap;
};

// prevent mw build error
struct vpss_chn_wrap_cfg {
	VPSS_GRP VpssGrp;
	VPSS_CHN VpssChn;
	VPSS_CHN_BUF_WRAP_S wrap;
};

/* prevent mw build error */
struct vpss_get_chn_frm_cfg {
	VPSS_GRP VpssGrp;
	VPSS_CHN VpssChn;
	VIDEO_FRAME_INFO_S stFrameInfo;
	CVI_S32 s32MilliSec;
};

struct cvi_vpss_vc_sb_cfg {
	__u8 img_inst;
	__u8 sc_inst;
	__u8 img_in_src_sel; /* 0: ISP, 2: DRAM */
	__u8 img_in_isp; /* 0: trig by reg_img_in_x_trl, 1: trig by isp vsync */
	__u32 img_in_width;
	__u32 img_in_height;
	__u32 img_in_fmt;
	__u64 img_in_address[3];
	__u32 odma_width;
	__u32 odma_height;
	__u32 odma_fmt;
	__u64 odma_address[3];
};

struct vpss_grp_csc_cfg {
	VPSS_GRP VpssGrp;
	__u16 coef[3][3];
	__u8 sub[3];
	__u8 add[3];
};

struct vpss_int_normalize {
	__u8 enable;
	__u16 sc_frac[3];
	__u8  sub[3];
	__u16 sub_frac[3];
	__u8 rounding;
};

struct vpss_vb_pool_cfg {
	VPSS_GRP VpssGrp;
	VPSS_CHN VpssChn;
	__u32 hVbPool;
};

struct vpss_snap_cfg {
	VPSS_GRP VpssGrp;
	VPSS_CHN VpssChn;
	__u32 frame_cnt;
};

struct vpss_bld_cfg {
	__u8 enable;
	__u8 fix_alpha;
	__u8 blend_y;
	__u8 y2r_enable;
	__u16 alpha_factor;
	__u16 alpha_stp;
	__u16 wd;
};


/* Public */
#define CVI_VPSS_CREATE_GROUP 0x00
#define CVI_VPSS_DESTROY_GROUP 0x01
#define CVI_VPSS_GET_AVAIL_GROUP 0x02
#define CVI_VPSS_START_GROUP 0x03
#define CVI_VPSS_STOP_GROUP 0x04
#define CVI_VPSS_RESET_GROUP 0x05
#define CVI_VPSS_SET_GRP_ATTR 0x06
#define CVI_VPSS_GET_GRP_ATTR 0x07
#define CVI_VPSS_SET_GRP_CROP 0x08
#define CVI_VPSS_GET_GRP_CROP 0x09
#define CVI_VPSS_GET_GRP_FRAME 0x0a
#define CVI_VPSS_SET_RELEASE_GRP_FRAME 0x0b
#define CVI_VPSS_SEND_FRAME 0x0c

#define CVI_VPSS_SEND_CHN_FRAME 0x20
#define CVI_VPSS_SET_CHN_ATTR 0x21
#define CVI_VPSS_GET_CHN_ATTR 0x22
#define CVI_VPSS_ENABLE_CHN 0x23
#define CVI_VPSS_DISABLE_CHN 0x24
#define CVI_VPSS_SET_CHN_CROP 0x25
#define CVI_VPSS_GET_CHN_CROP 0x26
#define CVI_VPSS_SET_CHN_ROTATION 0x27
#define CVI_VPSS_GET_CHN_ROTATION 0x28
#define CVI_VPSS_SET_CHN_LDC 0x29
#define CVI_VPSS_GET_CHN_LDC 0x2a
#define CVI_VPSS_GET_CHN_FRAME 0x2b
#define CVI_VPSS_RELEASE_CHN_RAME 0x2c
#define CVI_VPSS_SET_CHN_ALIGN 0x2d
#define CVI_VPSS_GET_CHN_ALIGN 0x2e
#define CVI_VPSS_SET_CHN_YRATIO 0x2f
#define CVI_VPSS_GET_CHN_YRATIO 0x30
#define CVI_VPSS_SET_CHN_SCALE_COEFF_LEVEL 0x31
#define CVI_VPSS_GET_CHN_SCALE_COEFF_LEVEL 0x32
#define CVI_VPSS_SHOW_CHN 0x33
#define CVI_VPSS_HIDE_CHN 0x34
#define CVI_VPSS_SET_CHN_BUF_WRAP 0x35
#define CVI_VPSS_GET_CHN_BUF_WRAP 0x036
#define CVI_VPSS_ATTACH_VB_POOL 0x37
#define CVI_VPSS_DETACH_VB_POOL 0x38
#define CVI_VPSS_TRIGGER_SNAP_FRAME 0x39

/* Internal use */
#define CVI_VPSS_SET_MODE 0x75
#define CVI_VPSS_SET_MODE_EX 0x76

#define CVI_VPSS_SET_GRP_CSC_CFG 0x78
#define CVI_VPSS_SET_BLD_CFG 0x79

int vpss_core_init(void);
int vpss_open(void);
int vpss_close(void);
void vpss_core_exit(void);

#ifdef __cplusplus
}
#endif

#endif /* _U_VPSS_UAPI_H_ */
