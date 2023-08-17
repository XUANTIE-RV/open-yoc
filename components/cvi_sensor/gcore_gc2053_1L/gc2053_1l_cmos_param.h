#ifndef __GC2053_1L_CMOS_PARAM_H_
#define __GC2053_1L_CMOS_PARAM_H_

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
#include "gc2053_1l_cmos_ex.h"

static const GC2053_1L_MODE_S g_stGc2053_1l_mode = {
	.name = "1920X1080P30",
	.stImg = {
		.stSnsSize = {
			.u32Width = 1920,
			.u32Height = 1080,
		},
		.stWndRect = {
			.s32X = 0,
			.s32Y = 0,
			.u32Width = 1920,
			.u32Height = 1080,
		},
		.stMaxSize = {
			.u32Width = 1920,
			.u32Height = 1080,
		},
	},
	.f32MaxFps = 30,
	.f32MinFps = 2.07, /* 1125 * 30 / 0x3FFF */
	.u32HtsDef = 2200,
	.u32VtsDef = 1125,
	.stExp = {
		.u16Min = 1,
		.u16Max = 0x3fff,
		.u16Def = 0x2000,
		.u16Step = 1,
	},
	.stAgain = {
		.u32Min = 64,
		.u32Max = 62977,
		.u32Def = 64,
		.u32Step = 1,
	},
	.stDgain = {
		.u32Min = 64*16,
		.u32Max = 7073*16,
		.u32Def = 581*16,
		.u32Step = 10*16,
	},
};

static ISP_CMOS_BLACK_LEVEL_S g_stIspBlcCalibratio = {
	.bUpdate = CVI_TRUE,
	.blcAttr = {
		.Enable = 1,
		.enOpType = OP_TYPE_AUTO,
		.stManual = {257, 257, 257, 257, 0, 0, 0, 0,
#ifdef ARCH_CV182X
			1093, 1093, 1093, 1093
#endif
		},
		.stAuto = {
			{257, 257, 257, 257, 259, 259, 260, 267, 278, 298, 366, 383, 366, 373, 372, 372 },
			{257, 257, 257, 257, 258, 259, 261, 266, 274, 297, 379, 377, 372, 365, 373, 374 },
			{257, 257, 257, 257, 258, 259, 261, 266, 275, 296, 376, 388, 366, 374, 376, 372 },
			{257, 257, 257, 257, 258, 259, 260, 264, 274, 294, 362, 363, 365, 361, 353, 367 },
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
#ifdef ARCH_CV182X
			{1093, 1093, 1093, 1093, 1093, 1093, 1093, 1095,
				1099, 1104, 1125, 1130, 1125, 1127, 1126, 1126},
			{1093, 1093, 1093, 1093, 1093, 1093, 1094, 1095,
				1097, 1104, 1128, 1128, 1126, 1124, 1127, 1127},
			{1093, 1093, 1093, 1093, 1093, 1093, 1094, 1095,
				1098, 1104, 1128, 1131, 1125, 1127, 1128, 1126},
			{1093, 1093, 1093, 1093, 1093, 1093, 1093, 1095,
				1097, 1103, 1123, 1124, 1124, 1123, 1121, 1125},
#endif
		},
	},
};

#if (CONFIG_BOARD_CV181XC || CONFIG_BOARD_CV1820_1821_1822)
struct combo_dev_attr_s gc2053_1l_rx_attr = {
	.input_mode = INPUT_MODE_MIPI,
	.mac_clk = RX_MAC_CLK_200M,
	.mipi_attr = {
		.raw_data_type = RAW_DATA_10BIT,
		.lane_id = {3, 4, -1, -1, -1},
		.wdr_mode = CVI_MIPI_WDR_MODE_NONE,
		.dphy = {
			.enable = 1,
			.hs_settle = 31,
		},
	},
	.mclk = {
		.cam = 1,
		.freq = CAMPLL_FREQ_27M,
	},
	.devno = 1,
};
#else
struct combo_dev_attr_s gc2053_1l_rx_attr = {
	.input_mode = INPUT_MODE_MIPI,
	.mac_clk = RX_MAC_CLK_200M,
	.mipi_attr = {
		.raw_data_type = RAW_DATA_10BIT,
		.lane_id = {3, 4, -1, -1, -1},
		.wdr_mode = CVI_MIPI_WDR_MODE_NONE,
		.dphy = {
			.enable = 1,
			.hs_settle = 31,
		},
	},
	.mclk = {
		.cam = 0,
		.freq = CAMPLL_FREQ_27M,
	},
	.devno = 1,
};
#endif
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /* __GC2053_1L_CMOS_PARAM_H_ */

