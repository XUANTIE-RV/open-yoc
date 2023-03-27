/* SPDX-License-Identifier: GPL-2.0-or-later
 * CV181x I2s driver on CVITEK CV181x
 *
 * Copyright 2022 CVITEK
 *
 * Author: Ruilong.Chen
 *
 */

#ifndef __DW_I2S_H_
#define _DW_I2S_H_
#include <stdbool.h>
#include <stdio.h>
#include "mmio.h"
#define AUDIO_DEBUG_CLI_SUPPORT

//#define __DEBUG__
#define USE_DMA_MODE
//#define ARCH_CV182X
#define ARCH_CV181X

#include "rtos_types.h"
//#include "cvi_printf.h"
#define INIT_FIFO_THRESHOLD      7
#define INIT_FIFO_HIGH_THRESHOLD 31


#ifdef __DEBUG__
#define debug printf
#else
#define debug(...)                                                             \
{                                                                      \
}
#endif

//#define printf cvi_printf

#define I2S0    0
#define I2S1    1
#define I2S2    2
#define I2S3    3

#define CVI_CONFIG_SYS_I2S0_BASE	0x04100000
#define CVI_CONFIG_SYS_I2S1_BASE	0x04110000
#define CVI_CONFIG_SYS_I2S2_BASE	0x04120000
#define CVI_CONFIG_SYS_I2S3_BASE	0x04130000

#define CVI_CONFIG_SYS_I2S_SYS_BASE	0x04108000

#define CVI_REG_AUDIO_GPIO_BASE		0x0300A12C
#define CVI_REG_AUDIO_SRC_BASE 		0x041D0000
#define CVI_REG_AUDIO_MISC_BASE 	0x041D0C00

#define CONFIG_SHIFT_HALF_T
//shit 1/2 T for inv, ex master generate data at falling edge and let codec sample at rising edge//
//#define CONFIG_I2S_USE_DMA
#define CVI_CONFIG_USE_AUDIO_PLL
#define ENABLE_DEBUG

#define CVI_I2S_BITS_PER_LONG 32
#define BIT(nr)			(1UL << (nr))
#define I2S_GENMASK(h, l) \
	(((~0UL) << (l)) & (~0UL >> (CVI_I2S_BITS_PER_LONG - 1 - (h))))

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

#define CVI_SKIP_TX_INACT_SLOT_MASK (0x1 << 5)
#define CVI_SKIP_TX_INACT_SLOT (0x1 << 5)
#define CVI_SKIP_RX_INACT_SLOT_MASK (0x1 << 4)
#define CVI_SKIP_RX_INACT_SLOT (0x1 << 4)

#define CVI_WORD_LEN_32         (0x2 << 1)
#define CVI_WORD_LEN_16         (0x1 << 1)
#define CVI_WORD_LEN_8          (0x0 << 1)
#define CVI_BCLK_OUT_FORCE_EN	(0x1 << 6)
//#define WORD_LENGTH_MASK    (0x3 << 1)


enum {
	CVI_PCM_STREAM_PLAYBACK = 0,
	CVI_PCM_STREAM_CAPTURE,
	CVI_PCM_STREAM_LAST = CVI_PCM_STREAM_CAPTURE,
};

struct i2s_tdm_regs {
	volatile unsigned int blk_mode_setting;		/* 0x00 */
	volatile unsigned int frame_setting;		/* 0x04 */
	volatile unsigned int slot_setting1;		/* 0x08 */
	volatile unsigned int slot_setting2;	    /* 0x0c */
	volatile unsigned int data_format;	    /* 0x10 */
	volatile unsigned int blk_cfg;	    /* 0x14 */
	volatile unsigned int i2s_enable;	    /* 0x18 */
	volatile unsigned int i2s_reset;      /* 0x1C */
	volatile unsigned int i2s_int_en;	/* 0x20 */
	volatile unsigned int i2s_int;	/* 0x24 */
	volatile unsigned int fifo_threshold;	        /* 0x28 */
	volatile unsigned int i2s_lrck_master; //dummy1[1];	/* 0x2c */
	volatile unsigned int fifo_reset;	        /* 0x30 */
	volatile unsigned int dummy2[3];	/* 0x34 ~ 0x3C */
	volatile unsigned int rx_status;	    /* 0x40 */
	volatile unsigned int dummy3[1];	/* 0x44 */
	volatile unsigned int tx_status;	    /* 0x48 */
	volatile unsigned int dummy4[5];	/* 0x4c ~ 0x5c */
	volatile unsigned int i2s_clk_ctrl0;		/* 0x60 */
	volatile unsigned int i2s_clk_ctrl1;	/* 0x64 */
	volatile unsigned int i2s_pcm_synth; /* 0x68 */
	volatile unsigned int dummy5[5];		/* 0x70 ~ 0x7c */
	volatile unsigned int rx_rd_port_ch0;	    /* 0x80 */
	volatile unsigned int rx_rd_port_ch1;	    /* 0x84 */
	volatile unsigned int dummy6[14];	    /* 0x88 ~ 0xbc */
	volatile unsigned int tx_wr_port_ch0;	    /* 0xc0 */
	volatile unsigned int tx_wr_port_ch1;	    /* 0xc4 */
};
//0x04108000
struct i2s_sys_regs {
	volatile unsigned int i2s_tdm_sclk_in_sel; /* 0x000 */
	volatile unsigned int i2s_tdm_fs_in_sel; /* 0x004 */
	volatile unsigned int i2s_tdm_sdi_in_sel; /* 0x008 */
	volatile unsigned int i2s_tdm_sdo_out_sel; /* 0x00C */
	volatile unsigned int dummy1[4];
	volatile unsigned int i2s_tdm_multi_sync; /* 0x020 */
	volatile unsigned int dummy2[3];
	volatile unsigned int i2s_bclk_oen_sel; /* 0x030 */
	volatile unsigned int i2s_bclk_out_ctrl; /* 0x034 */
	volatile unsigned int dummy3[2];
	volatile unsigned int audio_pdm_ctrl; /* 0x040 */
	volatile unsigned int dummy4[3];
	volatile unsigned int audio_phy_bypass1; /* 0x050 */
	volatile unsigned int audio_phy_bypass2; /* 0x054 */
	volatile unsigned int dummy5[6];
	volatile unsigned int i2s_sys_clk_ctrl; /* 0x070 */
	volatile unsigned int dummy6[3];
	volatile unsigned int i2s0_master_clk_ctrl0; /* 0x080 */
	volatile unsigned int i2s0_master_clk_ctrl1; /* 0x084 */
	volatile unsigned int dummy7[2];
	volatile unsigned int i2s1_master_clk_ctrl0; /* 0x090 */
	volatile unsigned int i2s1_master_clk_ctrl1; /* 0x094 */
	volatile unsigned int dummy8[2];
	volatile unsigned int i2s2_master_clk_ctrl0; /* 0x0A0 */
	volatile unsigned int i2s2_master_clk_ctrl1; /* 0x0A4 */
	volatile unsigned int dummy9[2];
	volatile unsigned int i2s3_master_clk_ctrl0; /* 0x0B0 */
	volatile unsigned int i2s3_master_clk_ctrl1; /* 0x0B4 */
	volatile unsigned int dummy10[2];
	volatile unsigned int i2s_sys_lrck_ctrl; /* 0x0C0 */
};

struct reg_audio_misc_regs {
	volatile unsigned int dummy[12];
	volatile unsigned int reg_pdm_en; /* 0x30 */
	volatile unsigned int reg_pdm_clk; /* 0x34 */
	volatile unsigned int reg_pdm_i2s; /* 0x38 */
};

struct reg_audio_src_regs {
	volatile unsigned int reg_src_en;	/* 0x00 */
	volatile unsigned int reg_src_sel; /* 0x04 */
	volatile unsigned int reg_src_setting; /* 0x08 */
	volatile unsigned int reg_src_format; /* 0x0c */
	volatile unsigned int dummy1[5];
	volatile unsigned int reg_src_ratio; /* 0x24 */
	volatile unsigned int reg_src_fsi_init; /* 0x28 */
	volatile unsigned int dummy2[1];
	volatile unsigned int reg_src_i2s_status; /* 0x30 */
	volatile unsigned int dummy3[11];
	volatile unsigned int reg_src_data_out; /* 0x60 */
	volatile unsigned int reg_dma_th; /* 0x64 */
};

/* This structure stores the i2s related information */
struct i2stx_info {
	volatile unsigned int rfs;		/* LR clock frame size */
	volatile unsigned int sclkg;		/* sclk gate */
	/* unsigned int audio_pll_clk;*/	/* Audio pll frequency in Hz */
	volatile unsigned int samplingrate;	/* sampling rate */
	volatile unsigned int mclk_out_en;
	volatile unsigned int clk_src;
	volatile unsigned int bitspersample;	/* bits per sample */
	volatile unsigned int channels;		/* audio channels */
	struct i2s_tdm_regs *base_address;	/* I2S Register Base */
	struct i2s_sys_regs *sys_base_address;
	volatile unsigned int id;		/* I2S controller id */
	volatile unsigned char role;     /* Master mode or slave mode*/
	volatile unsigned char slot_no;
	volatile unsigned int inv;       /* Normal or invert BCLK, normal or invert WS CLK (FSYNC)*/
	volatile unsigned char aud_mode; /*I2S mode, Left justified mode or Right justified mode*/
	volatile u16 mclk_div;
	volatile u16 bclk_div;
	volatile u16 sync_div;
	volatile unsigned char fifo_threshold;
	volatile unsigned char fifo_high_threshold;
};


#if defined(BOARD_FPGA)
#define I2S_TIMEOUT 50
#else
#define I2S_TIMEOUT 200
#endif

#define I2S_MODE            0x0
#define LJ_MODE             0x1
#define RJ_MODE             0x2
#define PCM_A_MODE          0x3
#define PCM_B_MODE          0x4
#define TDM_MODE            0x5

/* define value of each configuration of register BLK_MODE_SETTING */
#define RX_MODE	            0x0 << 0
#define TX_MODE             0x1 << 0
#define SLAVE_MODE	        0x0 << 1
#define MASTER_MODE	        0x1 << 1
#define RX_SAMPLE_EDGE_N    0x0 << 2 /* Negative edge */
#define RX_SAMPLE_EDGE_P    0x1 << 2 /* Positive edge */
#define TX_SAMPLE_EDGE_N    0x0 << 3
#define TX_SAMPLE_EDGE_P    0x1 << 3
#define FS_SAMPLE_EDGE_N    0x0 << 4
#define FS_SAMPLE_EDGE_P    0x1 << 4
#define FS_SAMPLE_RX_DELAY	0x1 << 5
#define FS_SAMPLE_RX_NO_DELAY	0x0 << 5
#define SW_MODE             0x0 << 7
#define HW_DMA_MODE         0x1 << 7
#define MULTI_I2S_SYNC      0x1 << 8
#define TXRX_MODE_MASK      0x00000001
#define ROLE_MASK           0x00000002
#define SAMPLE_EDGE_MASK    0x0000001C
#define FS_SAMPLE_RX_DELAY_MASK	0x00000020
#define DMA_MODE_MASK       0x00000080
#define MULTI_I2S_MODE_MASK 0x00000100

/* define value of each configuration of register FRAME_SETTING */
#define FS_POLARITY_MASK    0x00001000
#define FS_OFFSET_MASK      0x00002000
#define FS_IDEF_MASK        0x00004000
#define FS_ACT_LENGTH_MASK  0x00FF0000
#define FRAME_LENGTH_MASK   0x000001FF
#define FRAME_LENGTH(l)     (((l-1) << 0) & FRAME_LENGTH_MASK) /* frame length between 0~511 = 1~512 bits */
#define FS_ACT_LOW          0x0 << 12
#define FS_ACT_HIGH         0x1 << 12
#define NO_FS_OFFSET        0x0 << 13
#define FS_OFFSET_1_BIT     0x1 << 13
#define FS_IDEF_FRAME_SYNC         0x0 << 14 /* frame sync*/
#define FS_IDEF_CH_SYNC         0x1 << 14 /* channel sync */
#define FS_ACT_LENGTH(l)    (((l-1) << 16) & FS_ACT_LENGTH_MASK) /* frame active length between 0~255 = 1~256 bits*/


/* define value of each configuration of register SLOT_SETTING1 */

#define SLOT_NUM_MASK       0x0000000F
#define SLOT_SIZE_MASK      0x00003F00
#define DATA_SIZE_MASK      0x001F0000
#define FB_OFFSET_MASK      0x1F000000
#define SLOT_NUM(l)         (((l-1) << 0) & SLOT_NUM_MASK)
#define SLOT_SIZE(l)        (((l-1) << 8) & SLOT_SIZE_MASK)
#define DATA_SIZE(l)        (((l-1) << 16) & DATA_SIZE_MASK)
#define FB_OFFSET(l)        ((l << 24) & FB_OFFSET_MASK)

/* define value of each configuration of register DATA_FORMAT */
#define WORD_LENGTH_MASK    0x00000006


/* define value of each configuration of register BLK_CFG */
#define AUTO_DISABLE_W_CH_EN    0x1 << 4
#define RX_START_WAIT_DMA_EN	0x1 << 6

/* define value of each configuration of register I2S_RESET */
#define I2S_RESET_RX_PULL_UP 0x00000001
#define I2S_RESET_RX_PULL_DOWN 0x00000000
#define I2S_RESET_TX_PULL_UP 0x00000002
#define I2S_RESET_TX_PULL_DOWN 0x00000000

/* define value of each configuration of register I2S_INT_EN */
#define I2S_INT_EN_ALL		0x00000077

/* define value of each configuration of register I2S_INT */
#define I2S_INT_RXDA		0x1 << 0 /* RX FIFO data available interrupt status */
#define I2S_INT_RXFO		0x1 << 1 /* RX FIFO overflow interrupt status */
#define I2S_INT_RXFU		0x1 << 2 /* RX FIFO underflow interrupt status */
#define I2S_INT_TXDA		0x1 << 4 /* TX FIFO data available interrupt status */
#define I2S_INT_TXFO		0x1 << 5 /* TX FIFO overflow interrupt status */
#define I2S_INT_TXFU		0x1 << 6 /* TX FIFO underflow interrupt status */
#define I2S_INT_RXDA_RAW	0x1 << 8 /* RX FIFO data available interrupt raw status */
#define I2S_INT_RXFO_RAW	0x1 << 9 /* RX FIFO overflow interrupt raw status */
#define I2S_INT_RXFU_RAW	0x1 << 10 /* RX FIFO underflow interrupt raw status */
#define I2S_INT_TXDA_RAW	0x1 << 12 /* TX FIFO data available interrupt raw status */
#define I2S_INT_TXFO_RAW	0x1 << 13 /* TX FIFO overflow interrupt raw status */
#define I2S_INT_TXFU_RAW	0x1 << 14 /* TX FIFO underflow interrupt raw status */

/* define value of each configuration of register FIFO_THRESHOLD */
#define RX_FIFO_THRESHOLD_MASK  0x0000001F
#define TX_FIFO_THRESHOLD_MASK  0x001F0000
#define TX_FIFO_HIGH_THRESHOLD_MASK  0x1F000000
#define RX_FIFO_THRESHOLD(v)    ((v << 0) & RX_FIFO_THRESHOLD_MASK)
#define TX_FIFO_THRESHOLD(v)    ((v << 16) & TX_FIFO_THRESHOLD_MASK)
#define TX_FIFO_HIGH_THRESHOLD(v)    ((v << 24) & TX_FIFO_HIGH_THRESHOLD_MASK)

/* define value of each configuration of register FIFO_RESET */
#define RX_FIFO_RESET_PULL_UP 0x00000001
#define RX_FIFO_RESET_PULL_DOWN 0x00000000
#define TX_FIFO_RESET_PULL_UP 0x00010000
#define TX_FIFO_RESET_PULL_DOWN 0x00000000

/* define value of each configuration of register RX_STATUS */
#define RESET_RX_SCLK           0x00800000

/* define value of each configuration of register TX_STATUS */
#define RESET_TX_SCLK           0x00800000

/* define value of each configuration of register CLK_CTRL0 */
#define AUD_CLK_SOURCE_MASK     0x00000001
#define AUD_SWITCH              0x00000100
#define AUD_CLK_FROM_PLL        0x0 << 0
#define AUD_CLK_FROM_MCLK_IN    0x1 << 0
#define ADU_BCLK_OUT_EN		0x1 << 6
#define AUD_MCLK_OUT_EN		0x1 << 7
#define AUD_DISABLE             0x0 << 8
#define AUD_ENABLE              0x1 << 8


/* define value of each configuration of register CLK_CTRL1 */
#define MCLK_DIV(l)             ((l << 0) & 0x0000FFFF)
#define BCLK_DIV(l)             ((l << 16) & 0xFFFF0000)


#define FMT_IB_NF    0  /* sample at falling edge and sync polarity is active low*/
#define FMT_IB_IF    1
#define FMT_NB_NF    2
#define FMT_NB_IF    3




#define I2S_ON	1
#define I2S_OFF 0

/* I2S Tx Control */
#define I2S_TX_ON	1
#define I2S_TX_OFF	0

/* I2S Rx Control */
#define I2S_RX_ON	1
#define I2S_RX_OFF	0

#define WSS_16_CLKCYCLE   0x20
#define WSS_24_CLKCYCLE   0x30
#define WSS_32_CLKCYCLE   0x40
#define WSS_256_CLKCYCLE  0x200

#define AUD_SRC_EN		0x1 << 0
#define AUD_SRC_OFF		0x0 << 0

#define AUD_SRC_FSI(v)		((v << 0) & 0x000003FF)
#define AUD_SRC_FSO(v)		((v << 16) & 0x03FF0000)


struct i2s_tdm_regs *i2s_get_base(unsigned int i2s_no);
struct i2s_sys_regs *i2s_get_sys_base(void);
int i2s_get_no(unsigned int base_reg);

void i2s_set_clk_source(struct i2s_tdm_regs *i2s_reg, unsigned int src);
void i2s_set_mclk_out_enable(struct i2s_tdm_regs *i2s_reg, unsigned int enable);
void i2s_set_sample_rate(struct i2s_tdm_regs *i2s_reg, unsigned int sample_rate, u32 chan_nr, char *shortname);
void i2s0_set_clk_sample_rate(struct i2s_tdm_regs *i2s_reg, unsigned int sample_rate);

void i2s_set_ws_clock_cycle(struct i2s_tdm_regs *i2s_reg, unsigned int ws_clk, u8 aud_mode);
void i2s_set_resolution(struct i2s_tdm_regs *i2s_reg, unsigned int data_size, unsigned int slot_size);
int i2s_set_fmt(struct i2s_tdm_regs *i2s_reg,
                unsigned char role,
                unsigned char aud_mode,
                unsigned int fmt,
                unsigned char slot_no);

int i2s_init(struct i2stx_info *pi2s_tx);
int i2s_init_degbug(struct i2stx_info *pi2s_tx);

void i2s_loop_test(struct i2stx_info *pi2s_tx, unsigned int sec, unsigned int *data);
void i2s_pdm_loop_test(struct i2stx_info *pi2s_tx, unsigned int sec, unsigned int *data);
void i2s_src_test(struct i2stx_info *pi2s_tx);
int i2s_test_rx(struct i2stx_info *pi2s_tx, unsigned int resolution);

int i2s_receive_rx_data(struct i2stx_info *pi2s_tx);
int i2s_transfer_tx_data(struct i2stx_info *pi2s_tx);


void i2s_prepare_clk(void);
void i2s_switch(int on, struct i2s_tdm_regs *i2s_reg);
void i2s_sw_reset(struct i2s_tdm_regs *i2s_reg);
void i2s_set_interrupt(struct i2s_tdm_regs *i2s_reg, u32 stream);
void i2s_disable_all_interrupt(struct i2s_tdm_regs *i2s_reg);
void i2s_clear_irqs(struct i2s_tdm_regs *i2s_reg, u32 stream);
void i2s_disable_irqs(struct i2s_tdm_regs *i2s_reg, u32 stream);
void i2s_enable_irqs(struct i2s_tdm_regs *i2s_reg, u32 stream);

extern int i2s_debug_cmd_register(void);


/*
 * DAI hardware audio formats.
 *
 * Describes the physical PCM data formating and clocking. Add new formats
 * to the end.
 */
#define CVI_SND_SOC_DAIFMT_I2S			1 /* I2S mode */
#define CVI_SND_SOC_DAIFMT_RIGHT_J		2 /* Right Justified mode */
#define CVI_SND_SOC_DAIFMT_LEFT_J		3 /* Left Justified mode */
#define CVI_SND_SOC_DAIFMT_DSP_A		4 /* L data MSB after FRM LRC */
#define CVI_SND_SOC_DAIFMT_DSP_B		5 /* L data MSB during FRM LRC */
#define CVI_SND_SOC_DAIFMT_AC97			6 /* AC97 */
#define CVI_SND_SOC_DAIFMT_PDM			7 /* Pulse density modulation */

/* left and right justified also known as MSB and LSB respectively */
#define CVI_SND_SOC_DAIFMT_MSB		CVI_SND_SOC_DAIFMT_LEFT_J
#define CVI_SND_SOC_DAIFMT_LSB		CVI_SND_SOC_DAIFMT_RIGHT_J

/*
 * DAI hardware signal polarity.
 *
 * Specifies whether the DAI can also support inverted clocks for the specified
 * format.
 *
 * BCLK:
 * - "normal" polarity means signal is available at rising edge of BCLK
 * - "inverted" polarity means signal is available at falling edge of BCLK
 *
 * FSYNC "normal" polarity depends on the frame format:
 * - I2S: frame consists of left then right channel data. Left channel starts
 *      with falling FSYNC edge, right channel starts with rising FSYNC edge.
 * - Left/Right Justified: frame consists of left then right channel data.
 *      Left channel starts with rising FSYNC edge, right channel starts with
 *      falling FSYNC edge.
 * - DSP A/B: Frame starts with rising FSYNC edge.
 * - AC97: Frame starts with rising FSYNC edge.
 *
 * "Negative" FSYNC polarity is the one opposite of "normal" polarity.
 */
#define CVI_SND_SOC_DAIFMT_NB_NF		(0 << 8) /* normal bit clock + frame */
#define CVI_SND_SOC_DAIFMT_NB_IF		(2 << 8) /* normal BCLK + inv FRM */
#define CVI_SND_SOC_DAIFMT_IB_NF		(3 << 8) /* invert BCLK + nor FRM */
#define CVI_SND_SOC_DAIFMT_IB_IF		(4 << 8) /* invert BCLK + FRM */

#define CVI_SND_SOC_DAIFMT_CBM_CFM		(1 << 12) /* codec clk & FRM master */
#define CVI_SND_SOC_DAIFMT_CBS_CFM		(2 << 12) /* codec clk slave & FRM master */
#define CVI_SND_SOC_DAIFMT_CBM_CFS		(3 << 12) /* codec clk master & frame slave */
#define CVI_SND_SOC_DAIFMT_CBS_CFS		(4 << 12) /* codec clk & FRM slave */

#define CVI_SND_SOC_DAIFMT_FORMAT_MASK	0x000f
#define CVI_SND_SOC_DAIFMT_CLOCK_MASK	0x00f0
#define CVI_SND_SOC_DAIFMT_INV_MASK		0x0f00
#define CVI_SND_SOC_DAIFMT_MASTER_MASK	0xf000

#endif /* __DW_I2S_H_ */

