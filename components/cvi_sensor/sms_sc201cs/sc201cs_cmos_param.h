#ifndef __SC201CS_CMOS_PARAM_H_
#define __SC201CS_CMOS_PARAM_H_

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifdef ARCH_CV182X
#include "cvi_vip_cif_uapi.h"
#else
#include "cif_uapi.h"
#endif
#include "cvi_type.h"
#include "cvi_sns_ctrl.h"
#include "sc201cs_cmos_ex.h"

static const SC201CS_MODE_S g_astSc201cs_mode[SC201CS_MODE_NUM] = {
	[SC201CS_MODE_1600X1200P30] = {
		.name = "1600X1200P30",
		.astImg[0] = {
			.stSnsSize = {
				.u32Width = 1600,
				.u32Height = 1200,
			},
			.stWndRect = {
				.s32X = 0,
				.s32Y = 0,
				.u32Width = 1600,
				.u32Height = 1200,
			},
			.stMaxSize = {
				.u32Width = 1600,
				.u32Height = 1200,
			},
		},
		.f32MaxFps = 30,
		.f32MinFps = 2.28, /* 1250 * 30 / 0x3FFF */
		.u32HtsDef = 2560,
		.u32VtsDef = 1250,
		.stExp[0] = {
			.u16Min = 1,
			.u16Max = 1250 - 6,
			.u16Def = 100,
			.u16Step = 1,
		},
		.stAgain[0] = {
			.u32Min = 1024,
			.u32Max = 16384,
			.u32Def = 1024,
			.u32Step = 1,
		},
		.stDgain[0] = {
			.u32Min = 1024,
			.u32Max = 8064,
			.u32Def = 1024,
			.u32Step = 1,
		},
	},
};

static ISP_CMOS_BLACK_LEVEL_S g_stIspBlcCalibratio = {
	.bUpdate = CVI_TRUE,
	.blcAttr = {
		.Enable = 1,
		.enOpType = OP_TYPE_AUTO,
		.stManual = {260, 260, 260, 260, 0, 0, 0, 0
#ifdef ARCH_CV182X
		, 1093, 1093, 1093, 1093
#endif
		},
		.stAuto = {
			{260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260 },
			{260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260 },
			{260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260 },
			{260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260 },
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
#ifdef ARCH_CV182X
			{1093, 1093, 1093, 1093, 1093, 1093, 1093, 1093, 1093, 1093, 1093, 1093, 1093, 1093,
				1093, 1093},
			{1093, 1093, 1093, 1093, 1093, 1093, 1093, 1093, 1093, 1093, 1093, 1093, 1093, 1093,
				1093, 1093},
			{1093, 1093, 1093, 1093, 1093, 1093, 1093, 1093, 1093, 1093, 1093, 1093, 1093, 1093,
				1093, 1093},
			{1093, 1093, 1093, 1093, 1093, 1093, 1093, 1093, 1093, 1093, 1093, 1093, 1093, 1093,
				1093, 1093},
#endif
		},
	},
};

struct combo_dev_attr_s sc201cs_rx_attr = {
	.input_mode = INPUT_MODE_MIPI,
	.mac_clk = RX_MAC_CLK_200M,
	.mipi_attr = {
		.raw_data_type = RAW_DATA_10BIT,
		.lane_id = {0, 1, -1, -1, -1},
		.wdr_mode = CVI_MIPI_WDR_MODE_NONE,
	},
	.mclk = {
		.cam = 1,
		.freq = CAMPLL_FREQ_24M,
	},
	.devno = 0,
};

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /* __SC201CS_CMOS_PARAM_H_ */

