#ifndef _MIPI_TX_PARAM_ILI9488_H_
#define _MIPI_TX_PARAM_ILI9488_H_

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

static struct combo_dev_cfg_s dev_cfg_ili9488_320x480 = {
	.devno = 0,
	// .lane_id = {MIPI_TX_LANE_CLK, MIPI_TX_LANE_0, -1, -1, -1}, //EVB board
	// .lane_pn_swap = {true, true, true, true, true}, //EVB board
	.lane_id = {-1, MIPI_TX_LANE_CLK, MIPI_TX_LANE_0, -1, -1},
	.lane_pn_swap = {false, false, false, false, false},
	.output_mode = OUTPUT_MODE_DSI_VIDEO,
	.video_mode = BURST_MODE,
	.output_format = OUT_FORMAT_RGB_24_BIT,
	.sync_info = {
		.vid_hsa_pixels = 30,
		.vid_hbp_pixels = 70,
		.vid_hfp_pixels = 170,
		.vid_hline_pixels = 320,
		.vid_vsa_lines = 2,
		.vid_vbp_lines = 2,
		.vid_vfp_lines = 4,
		.vid_active_lines = 480,
		.vid_vsa_pos_polarity = false,
		.vid_hsa_pos_polarity = false,
	},
	.pixel_clk = 20000,
	.reset_pin = {
		// .gpio_num = CVI_GPIOE_02, //EVB board
		.gpio_num = CVI_GPIOA_28,
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

const struct hs_settle_s hs_timing_cfg_ili9488_320x480 = { .prepare = 6, .zero = 32, .trail = 1 };

static CVI_U8 data_ili9488_0[] = { 0xF7, 0xA9, 0x51, 0x2C, 0x82 };
static CVI_U8 data_ili9488_1[] = { 0x36, 0x48 };
static CVI_U8 data_ili9488_2[] = { 0x3A, 0x55 };
static CVI_U8 data_ili9488_3[] = { 0xB4, 0x02 };
static CVI_U8 data_ili9488_4[] = { 0xB1, 0xA0, 0x11 }; //FRAME RATE
/* Power Control 1   Vreg1out=4.56  Vreg2out=-4.56 */
static CVI_U8 data_ili9488_5[] = { 0xC0, 0x0F, 0x0F };
/* Power Control 2    VGH=15.81 ,VGL=-10.41,DDVDH=5.35，DDVDL=-5.23  VCL=-2.7 */
static CVI_U8 data_ili9488_6[] = { 0xC1, 0x41 };
static CVI_U8 data_ili9488_7[] = { 0xC2, 0x22 };
static CVI_U8 data_ili9488_8[] = { 0xB7, 0xC6 };
static CVI_U8 data_ili9488_9[] = { 0xC5, 0x00, 0x53, 0x80}; //DC VCOM
static CVI_U8 data_ili9488_10[] = { 0xBE, 0x00, 0x4};
static CVI_U8 data_ili9488_11[] = { 0xE9, 0x00};

static CVI_U8 data_ili9488_12[] = {
	0xE0, 0x00, 0x08, 0x0C, 0x02, 0x0E, 0x04, 0x30, 0x45, 0x47,
	0x04, 0x0C, 0x0A, 0x2E, 0x34, 0x0F
};

static CVI_U8 data_ili9488_13[] = {
	0xE1, 0x00, 0x11, 0x0D, 0x01, 0x0F, 0x05, 0x39, 0x36, 0x51,
	0x06, 0x0F, 0x0D, 0x33, 0x37, 0x0F
};

static CVI_U8 data_ili9488_14[] = { 0x21 };
static CVI_U8 data_ili9488_15[] = { 0x11 };
static CVI_U8 data_ili9488_16[] = { 0x29 };

const struct dsc_instr dsi_init_cmds_ili9488_320x480[] = {
	{.delay = 0, .data_type = 0x29, .size = sizeof(data_ili9488_0), .data = data_ili9488_0 },
	{.delay = 0, .data_type = 0x15, .size = sizeof(data_ili9488_1), .data = data_ili9488_1 },
	{.delay = 0, .data_type = 0x15, .size = sizeof(data_ili9488_2), .data = data_ili9488_2 },
	{.delay = 0, .data_type = 0x15, .size = sizeof(data_ili9488_3), .data = data_ili9488_3 },
	{.delay = 0, .data_type = 0x29, .size = sizeof(data_ili9488_4), .data = data_ili9488_4 },
	{.delay = 0, .data_type = 0x29, .size = sizeof(data_ili9488_5), .data = data_ili9488_5 },
	{.delay = 0, .data_type = 0x15, .size = sizeof(data_ili9488_6), .data = data_ili9488_6 },
	{.delay = 0, .data_type = 0x15, .size = sizeof(data_ili9488_7), .data = data_ili9488_7 },
	{.delay = 0, .data_type = 0x15, .size = sizeof(data_ili9488_8), .data = data_ili9488_8 },
	{.delay = 0, .data_type = 0x29, .size = sizeof(data_ili9488_9), .data = data_ili9488_9 },
	{.delay = 0, .data_type = 0x29, .size = sizeof(data_ili9488_10), .data = data_ili9488_10 },
	{.delay = 0, .data_type = 0x15, .size = sizeof(data_ili9488_11), .data = data_ili9488_11 },
	{.delay = 0, .data_type = 0x29, .size = sizeof(data_ili9488_12), .data = data_ili9488_12 },
	{.delay = 0, .data_type = 0x29, .size = sizeof(data_ili9488_13), .data = data_ili9488_13 },
	{.delay = 0, .data_type = 0x05, .size = sizeof(data_ili9488_14), .data = data_ili9488_14 },
	{.delay = 150, .data_type = 0x05, .size = sizeof(data_ili9488_15), .data = data_ili9488_15 },
	{.delay = 0, .data_type = 0x05, .size = sizeof(data_ili9488_16), .data = data_ili9488_16 },

};

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#else
#error "MIPI_TX_PARAM multi-delcaration!!"
#endif // _MIPI_TX_PARAMILI_9488_H_
