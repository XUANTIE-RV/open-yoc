#ifndef _SILAN_I2S_H_
#define _SILAN_I2S_H_

#if defined(__CC_ARM)
#include "silan_core_m0.h"
#endif

#include "silan_types.h"
#include "silan_codec.h"

#define I2S_BASE_ADDR		APER_APB_I2S_I1_BASE

#define I2S_FIFO_DEPTH		32

//iis ctrl
#define RX_UNITE_EN    		 (1<<31)
#define TX_UNITE_EN    		 (1<<26)
#define TX_FLUSH       		 (1<<20)
#define ITX_EN        		 (1<<16)
#define RX_FLUSH      		 (1<<12)
#define IRX_EN        		 (1<<8)
#define I2S_EN        		 (1<<0)

#define I2S_RESET           0X72778841

typedef enum I2S_id
{
	I2S_ID_I1		= 0,
	I2S_ID_I2,
	I2S_ID_I3,
	I2S_ID_O1,
	I2S_ID_O2,
	I2S_ID_O3,
	I2S_ID_NONE,
	I2S_ID_PCM,
	I2S_ID_LMT,
}
I2S_ID, *PI2S_ID;

typedef enum I2S_ch_mode
{
	I2S_CH_20		= 0,
	I2S_CH_40,
	I2S_CH_51,
	I2S_CH_71,
}
I2S_CH_MODE, *PI2S_CH_MODE;

typedef enum I2S_prot_mode
{
	I2S_PROT_LEFT	= 0,
	I2S_PROT_RIGHT,
	I2S_PROT_I2S,
	I2S_PROT_PCM,
}
I2S_PROT_MODE, *PI2S_PROT_MODE;

typedef enum I2S_ws
{
	I2S_WS_16	= 0,
	I2S_WS_24,
	I2S_WS_32,
}
I2S_WS, *PI2S_WS;

typedef enum I2S_wlen
{
	I2S_WLEN_NONE	= 0,
	I2S_WLEN_8,
	I2S_WLEN_12,
	I2S_WLEN_16,
	I2S_WLEN_20,
	I2S_WLEN_24,
	I2S_WLEN_32,
}
I2S_WLEN, *PI2S_WLEN;

typedef enum I2S_tr_mode
{
	I2S_TR_NONE	= 0,
	I2S_TR_TO,
	I2S_TR_RO,
	I2S_TR_DUAL,
}
I2S_TR_MODE, *PI2S_TR_MODE;

typedef struct I2S_chnl_regs
{
	__IO uint32_t		RFR;
	__IO uint32_t		TFR;
	__IO uint32_t		RCR;
	__IO uint32_t		TCR;
	__IO uint32_t		ISR;
	__IO uint32_t		IMR;
	__IO uint32_t		TXFIFO;
	__IO uint32_t		RXFIFO;
}
I2S_CHNL_REGS, *PI2S_CHNL_REGS;

typedef struct I2S_core_regs
{
	__IO uint32_t		I2S_CTRL;
	__IO uint32_t		CLK_CTRL;
	__IO uint32_t		NO_DEF;
	__IO uint32_t		ISRG;
	I2S_CHNL_REGS		I2S_CHNL[4];
	__IO uint32_t		SRESET;
}
I2S_CORE_REGS, *PI2S_CORE_REGS;

typedef struct I2S_cfg
{
	I2S_ID			id;
	I2S_CH_MODE		ch;
	I2S_PROT_MODE	prot;
	I2S_WS			ws;
	I2S_WLEN		wlen;
	uint8_t			burst;
	CODEC_ID		codec;
	CLK_MODE		master;
	uint8_t			tx_srcid;
	uint8_t			rx_srcid;
	I2S_TR_MODE		tr;
	I2S_CORE_REGS	*regs;
}
I2S_CFG, *PI2S_CFG;

void silan_i2s_start(I2S_CFG *pi2s_cfg);
int32_t silan_i2s_dev_open(I2S_CFG *pi2s_cfg);
void silan_i2s_stop(I2S_CFG *pi2s_cfg);

#endif

