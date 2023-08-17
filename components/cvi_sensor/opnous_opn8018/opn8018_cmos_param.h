#ifndef __OPN8018_CMOS_PARAM_H_
#define __OPN8018_CMOS_PARAM_H_

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
#include "opn8018_cmos_ex.h"

static const OPN8018_MODE_S g_astOpn8018_mode[OPN8018_MODE_NUM] = {
	[OPN8018_MODE_640X480P120] = {
		.name = "640X480P120",
		.astImg[0] = {
			.stSnsSize = {
				.u32Width = 648,
				.u32Height = 1464,
			},
			.stWndRect = {
				.s32X = 0,
				.s32Y = 0,
				.u32Width = 648,
				.u32Height = 1464,
			},
			.stMaxSize = {
				.u32Width = 648,
				.u32Height = 1464,
			},
		},
		.f32MaxFps = 120,
		.f32MinFps = 0.79, /* 0x20a * 100 / 0xFFFF */
		.u32HtsDef = 1522,
		.u32VtsDef = 538,
		.stExp[0] = {
			.u16Min = 2,
			.u16Max = 538 - 20,
			.u16Def = 100,
			.u16Step = 1,
		},
		.stAgain[0] = {
			.u16Min = 1024,
			.u16Max = 15872,
			.u16Def = 1024,
			.u16Step = 1,
		},
		.stDgain[0] = {
			.u16Min = 1024,
			.u16Max = 1024,
			.u16Def = 1024,
			.u16Step = 1,
		},
	},
};

static ISP_CMOS_BLACK_LEVEL_S g_stIspBlcCalibratio = {
	.bUpdate = CVI_TRUE,
	.blcAttr = {
		.Enable = 1,
		.enOpType = OP_TYPE_AUTO,
		.stManual = {64, 64, 64, 64, 0, 0, 0, 0
#ifdef ARCH_CV182X
			, 1040, 1040, 1040, 1040
#endif
		},
		.stAuto = {
			{64, 64, 64, 64, 64, 64, 64, 64, /*8*/64, 64, 64, 64, 64, 64, 64, 64},
			{64, 64, 64, 64, 64, 64, 64, 64, /*8*/64, 64, 64, 64, 64, 64, 64, 64},
			{64, 64, 64, 64, 64, 64, 64, 64, /*8*/64, 64, 64, 64, 64, 64, 64, 64},
			{64, 64, 64, 64, 64, 64, 64, 64, /*8*/64, 64, 64, 64, 64, 64, 64, 64},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
#ifdef ARCH_CV182X
			{1040, 1040, 1040, 1040, 1040, 1040, 1040, 1040,
				/*8*/1040, 1040, 1040, 1040, 1040, 1040, 1040, 1040},
			{1040, 1040, 1040, 1040, 1040, 1040, 1040, 1040,
				/*8*/1040, 1040, 1040, 1040, 1040, 1040, 1040, 1040},
			{1040, 1040, 1040, 1040, 1040, 1040, 1040, 1040,
				/*8*/1040, 1040, 1040, 1040, 1040, 1040, 1040, 1040},
			{1040, 1040, 1040, 1040, 1040, 1040, 1040, 1040,
				/*8*/1040, 1040, 1040, 1040, 1040, 1040, 1040, 1040},
#endif
		},
	},
};

struct combo_dev_attr_s opn8018_rx_attr = {
	.input_mode = INPUT_MODE_MIPI,
	.mac_clk = RX_MAC_CLK_600M,
	.mipi_attr = {
		.raw_data_type = RAW_DATA_12BIT,
		.lane_id = {2, 3, 1, -1, -1},
		.pn_swap = {1, 1, 1, 0, 0},
		.wdr_mode = CVI_MIPI_WDR_MODE_NONE,
		.dphy = {
			.enable = 1,
			.hs_settle = 8,
		},
	},
	.mclk = {
		.cam = 1,
		.freq = CAMPLL_FREQ_NONE,
	},
	.devno = 0,
};

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /* __OPN8018_CMOS_PARAM_H_ */
