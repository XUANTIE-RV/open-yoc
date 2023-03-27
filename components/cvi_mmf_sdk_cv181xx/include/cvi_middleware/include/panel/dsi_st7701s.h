#ifndef _MIPI_TX_PARAM_ST7701S_H_
#define _MIPI_TX_PARAM_ST7701S_H_

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

static struct combo_dev_cfg_s dev_cfg_st7701s_480x480 = {
	.devno = 0,
	.lane_id = {MIPI_TX_LANE_0, MIPI_TX_LANE_CLK, MIPI_TX_LANE_1, -1, -1},
	.lane_pn_swap = {false, false, false, false, false},
	.output_mode = OUTPUT_MODE_DSI_VIDEO,
	.video_mode = BURST_MODE,
	.output_format = OUT_FORMAT_RGB_24_BIT,
	.sync_info = {
		.vid_hsa_pixels = 8,
		.vid_hbp_pixels = 80,
		.vid_hfp_pixels = 80,
		.vid_hline_pixels = 480,
		.vid_vsa_lines = 2,
		.vid_vbp_lines = 30,
		.vid_vfp_lines = 18,
		.vid_active_lines = 480,
		.vid_vsa_pos_polarity = false,
		.vid_hsa_pos_polarity = true,
	},
	.pixel_clk = 20606,
	.reset_pin = {
		.gpio_num = CVI_GPIOE_12,
		.active = 0
	},
	.power_ct_pin = {
		.gpio_num = -1,
		.active = 1
	},
	.pwm_pin = {
		.gpio_num = -1,
		.active = 1
	}
};

// #define PANEL_BIST_MODE

const struct hs_settle_s hs_timing_cfg_st7701s_480x480 = { .prepare = 6, .zero = 32, .trail = 1 };

static CVI_U8 data_st7701s_0[] = { 0xff, 0x77, 0x01, 0x00, 0x00, 0x10 };
static CVI_U8 data_st7701s_1[] = { 0xc0, 0x3b, 0x00 };
static CVI_U8 data_st7701s_2[] = { 0xc1, 0x0d, 0x02 };
static CVI_U8 data_st7701s_3[] = { 0xc2, 0x21, 0x08 };
static CVI_U8 data_st7701s_4[] = {
	0xb0, 0x00, 0x11, 0x18, 0x0E, 0x11, 0x06, 0x07, 0x08, 0x07,
	0x22, 0x04, 0x12, 0x0F, 0xAA, 0x31, 0x18
};
static CVI_U8 data_st7701s_5[] = {
	0xB1, 0x00, 0x11, 0x19, 0x0E, 0x12, 0x07, 0x08, 0x08, 0x08,
	0x22, 0x04, 0x11, 0x11, 0xA9, 0x32, 0x18
};
static CVI_U8 data_st7701s_6[] = { 0xFF, 0x77, 0x01, 0x00, 0x00, 0x11};
static CVI_U8 data_st7701s_7[] = { 0xB0, 0x60};
static CVI_U8 data_st7701s_8[] = { 0xB1, 0x30};
static CVI_U8 data_st7701s_9[] = { 0xB2, 0x87};
static CVI_U8 data_st7701s_10[] = { 0xB3, 0x80};
static CVI_U8 data_st7701s_11[] = { 0xB5, 0x49 };
static CVI_U8 data_st7701s_12[] = { 0xB7, 0x85 };
static CVI_U8 data_st7701s_13[] = { 0xB8, 0x21 };
static CVI_U8 data_st7701s_14[] = { 0xC1, 0x78 };
static CVI_U8 data_st7701s_15[] = { 0xC2, 0x78 };
static CVI_U8 data_st7701s_16[] = { 0xE0, 0x00, 0x1B, 0x02 };
static CVI_U8 data_st7701s_17[] = {
	0xE1, 0x08, 0xA0, 0x00, 0x00, 0x07, 0xA0, 0x00, 0x00, 0x00,
	0x44, 0x44 };
static CVI_U8 data_st7701s_18[] = {
	0xE2, 0x11, 0x11, 0x44, 0x44, 0xED, 0xA0, 0x00, 0x00, 0xEC,
	0xA0, 0x00, 0x00
};
static CVI_U8 data_st7701s_19[] = { 0xE3, 0x00, 0x00, 0x11, 0x11 };
static CVI_U8 data_st7701s_20[] = { 0xE4, 0x44, 0x44 };
static CVI_U8 data_st7701s_21[] = {
	0xE5, 0x0A, 0xE9, 0xD8, 0xA0, 0x0C, 0xEB, 0xD8, 0xA0, 0x0E,
	0xED, 0xD8, 0xA0, 0x10, 0xEF, 0xD8, 0xA0
};
static CVI_U8 data_st7701s_22[] = { 0xE6, 0x00, 0x00, 0x11, 0x11 };
static CVI_U8 data_st7701s_23[] = { 0xE7, 0x44, 0x44 };
static CVI_U8 data_st7701s_24[] = {
	0xE8, 0x09, 0xE8, 0xD8, 0xA0, 0x0B, 0xEA, 0xD8, 0xA0, 0x0D,
	0xEC, 0xD8, 0xA0, 0x0F, 0xEE, 0xD8, 0xA0
};
static CVI_U8 data_st7701s_25[] = { 0xEB, 0x02, 0x00, 0xE4, 0xE4, 0x88, 0x00, 0x40 };
static CVI_U8 data_st7701s_26[] = { 0xEC, 0x3C, 0x00 };
static CVI_U8 data_st7701s_27[] = {
	0xED, 0xAB, 0x89, 0x76, 0x54, 0x02, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0x20, 0x45, 0x67, 0x98, 0xBA
};
#ifdef PANEL_BIST_MODE
static CVI_U8 data_st7701s_100[] = { 0xFF, 0x77, 0x01, 0x00, 0x00, 0x12 };
static CVI_U8 data_st7701s_101[] = { 0xD1, 0x81 };
static CVI_U8 data_st7701s_102[] = { 0xD2, 0x08 };
#endif
static CVI_U8 data_st7701s_28[] = { 0xFF, 0x77, 0x01, 0x00, 0x00, 0x00 };
static CVI_U8 data_st7701s_29[] = { 0x36, 0x00 };
static CVI_U8 data_st7701s_30[] = { 0x11 };
static CVI_U8 data_st7701s_31[] = { 0x29 };

const struct dsc_instr dsi_init_cmds_st7701s_480x480[] = {
	{.delay = 0, .data_type = 0x39, .size = 6, .data = data_st7701s_0 },
	{.delay = 0, .data_type = 0x39, .size = 3, .data = data_st7701s_1 },
	{.delay = 0, .data_type = 0x39, .size = 3, .data = data_st7701s_2 },
	{.delay = 0, .data_type = 0x39, .size = 3, .data = data_st7701s_3 },
	{.delay = 0, .data_type = 0x39, .size = 17, .data = data_st7701s_4 },
	{.delay = 0, .data_type = 0x39, .size = 17, .data = data_st7701s_5 },
	{.delay = 0, .data_type = 0x39, .size = 6, .data = data_st7701s_6 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_st7701s_7 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_st7701s_8 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_st7701s_9 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_st7701s_10 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_st7701s_11 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_st7701s_12 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_st7701s_13 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_st7701s_14 },
	{.delay = 20, .data_type = 0x15, .size = 2, .data = data_st7701s_15 },
	{.delay = 0, .data_type = 0x39, .size = 4, .data = data_st7701s_16 },
	{.delay = 0, .data_type = 0x39, .size = 12, .data = data_st7701s_17 },
	{.delay = 0, .data_type = 0x39, .size = 13, .data = data_st7701s_18 },
	{.delay = 0, .data_type = 0x39, .size = 5, .data = data_st7701s_19 },
	{.delay = 0, .data_type = 0x39, .size = 3, .data = data_st7701s_20 },
	{.delay = 0, .data_type = 0x39, .size = 17, .data = data_st7701s_21 },
	{.delay = 0, .data_type = 0x39, .size = 5, .data = data_st7701s_22 },
	{.delay = 0, .data_type = 0x39, .size = 3, .data = data_st7701s_23 },
	{.delay = 0, .data_type = 0x39, .size = 17, .data = data_st7701s_24 },
	{.delay = 0, .data_type = 0x39, .size = 8, .data = data_st7701s_25 },
	{.delay = 0, .data_type = 0x39, .size = 3, .data = data_st7701s_26 },
	{.delay = 0, .data_type = 0x39, .size = 17, .data = data_st7701s_27 },
#ifdef PANEL_BIST_MODE
	{.delay = 0, .data_type = 0x39, .size = 6, .data = data_st7701s_100 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_st7701s_101 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_st7701s_102 },
#endif
	{.delay = 0, .data_type = 0x39, .size = 6, .data = data_st7701s_28 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_st7701s_29 },
	{.delay = 200, .data_type = 0x05, .size = 1, .data = data_st7701s_30 },
	{.delay = 50, .data_type = 0x05, .size = 1, .data = data_st7701s_31 }

};

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#else
#error "MIPI_TX_PARAM multi-delcaration!!"
#endif // _MIPI_TX_PARAM_ST7701S_H_
