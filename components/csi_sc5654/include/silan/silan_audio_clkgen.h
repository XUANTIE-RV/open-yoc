#ifndef _SILAN_AUDIO_CLKGEN_H_
#define _SILAN_AUDIO_CLKGEN_H_

#include "silan_syscfg.h"
#include "silan_pmu.h"
#include "silan_i2s.h"

typedef union AUDIO_route_reg1 {
	/** raw data */
	uint32_t d32;
	/** bits def */
	struct {
#define ROUTE_I2S_SEL_CLKGEN    0
#define ROUTE_I2S_SEL_CODEC1    1
#define ROUTE_I2S_SEL_CODEC2    2
#define ROUTE_I2S_SEL_EXT	    3
		unsigned i2s_o1_sel:2;
		unsigned i2s_o1_ext_master:1;
		unsigned i2s_o2_sel:2;
		unsigned i2s_o2_ext_master:1;
		unsigned i2s_o3_sel:2;
		unsigned i2s_o3_ext_master:1;
		unsigned i2s_i1_sel:2;
		unsigned i2s_i1_ext_master:1;
		unsigned i2s_i2_sel:2;
		unsigned i2s_i2_ext_master:1;
		unsigned i2s_i3_sel:2;
		unsigned i2s_i3_ext_master:1;
		unsigned i2s_pcm_ext_master:1;
		unsigned codec1_i2s_loop:1;
		unsigned codec2_i2s_loop:1;
		unsigned dac2_sel:1;
		unsigned adc2_sel:1;
		unsigned reserved1:1;
		unsigned i2s_o1_sclk_inv:1;
		unsigned i2s_o2_sclk_inv:1;
		unsigned i2s_o3_sclk_inv:1;
		unsigned i2s_i1_sclk_inv:1;
		unsigned i2s_i2_sclk_inv:1;
		unsigned i2s_i3_sclk_inv:1;
		unsigned i2s_pcm_sclk_inv:1;
		unsigned reserved2:1;
	} __attribute__((packed)) b;
} __attribute__((packed)) AUDIO_ROUTE_REG1;

typedef union AUDIO_route_reg2 {
	/** raw data */
	uint32_t d32;
	/** bits def */
	struct {
#define ROUTE_CODEC_SEL_CLKGEN    0
#define ROUTE_CODEC_SEL_EXT       1
#define ROUTE_CODEC_SEL_LOOP      2
		unsigned codec1_dac_sel:2;
		unsigned codec1_adc_sel:2;
		unsigned codec2_dac_sel:2;
		unsigned codec2_adc_sel:2;
		unsigned i2s_i1_dly_sel:1;
		unsigned reserved1:22;
		unsigned codec_hd_cfg;
	} __attribute__((packed)) b;
} __attribute__((packed)) AUDIO_ROUTE_REG2;

void silan_i2s_clock_config(I2S_ID i2s_id, int rate);
int32_t silan_i2s_route_config(I2S_ID i2s_id, CODEC_ID codec_id, CLK_MODE master);

#endif
