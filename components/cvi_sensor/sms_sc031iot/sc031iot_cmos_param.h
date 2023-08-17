#ifndef __SC031IOT_CMOS_PARAM_H_
#define __SC031IOT_CMOS_PARAM_H_

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
#include "sc031iot_cmos_ex.h"

static const SC031IOT_MODE_S g_astSc031iot_mode[SC031IOT_MODE_NUM] = {
	[SC031IOT_MODE_640X480P30] = {
		.name = "640X480P30",
		.astImg[0] = {
			.stSnsSize = {
				.u32Width = 640,
				.u32Height = 480,
			},
			.stWndRect = {
				.s32X = 0,
				.s32Y = 0,
				.u32Width = 640,
				.u32Height = 480,
			},
			.stMaxSize = {
				.u32Width = 640,
				.u32Height = 480,
			},
		},
	},
};

struct combo_dev_attr_s sc031iot_rx_attr = {
	.input_mode = INPUT_MODE_MIPI,
	.mac_clk = RX_MAC_CLK_200M,
	.mipi_attr = {
		.raw_data_type = YUV422_8BIT,
		.lane_id = {2, 1, -1, -1, -1},
		.wdr_mode = CVI_MIPI_WDR_MODE_NONE,
		.dphy = {
			.enable = 1,
			.hs_settle = 8,
		}
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


#endif /* __SC031IOT_CMOS_PARAM_H_ */

