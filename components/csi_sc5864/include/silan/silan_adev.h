#ifndef _SILAN_ADEV_H_
#define _SILAN_ADEV_H_

#include "silan_i2s.h"
// #include "silan_spdif_in.h"
#include "silan_codec.h"
#include "silan_dmac.h"
#include "silan_dmac_regs.h"

#include "silan_config.h"

#ifndef ADEV_USER_CONFIG

#define ADEV_NOT_USE_SWAP

// 16bit@16KHz
#define ADEV_MAX_DMAC_CHNL		4
#define ADEV_MAX_DATA_WIDTH		16
#define ADEV_MAX_AUDIO_CHNL		2
#define ADEV_MAX_MULTIPLE_16K	1
#define ADEV_LLI_NUM			(50)
#define ADEV_LLI_BUF_NUM		(200)

#endif

#define LLI_SIZE_PER_1MS		16000/1000*ADEV_MAX_DATA_WIDTH/8*ADEV_MAX_AUDIO_CHNL*ADEV_MAX_MULTIPLE_16K

#define ADEV_MAX_LLI_SIZE		LLI_SIZE_PER_1MS

typedef struct ADEV_MONI_info
{
	uint32_t		last_ccount;
	int32_t			diff_ccount;
	uint32_t		intr_num;
	uint32_t		intr_cnt;
	uint32_t		intr_total;
	uint32_t		buf_fullness;
}
ADEV_MONI_INFO, *PADEV_MONI_INFO;

typedef struct ADEV_cfg
{
	DMA_REQUEST		*tx_dma_req;
	DMA_REQUEST		*rx_dma_req;
	uint8_t			width;
	uint8_t			burst;
	uint32_t		sample_rate;
	int8_t			tx_num;
	int8_t			rx_num;
	ADEV_MONI_INFO	*tx_info;
	ADEV_MONI_INFO	*rx_info;
}
ADEV_CFG, *PADEV_CFG;

typedef struct ADEV_I2S_handle
{
	ADEV_CFG		adev_cfg;
	I2S_CFG			i2s_cfg;
	CODEC_CFG		codec_cfg;
	char            *buf;
	char            *buf_align;

	int             remain_size;
	char            remain_buf[48000/1000*(16/8)*2];
}
ADEV_I2S_HANDLE, *PADEV_I2S_HANDLE;

//typedef struct ADEV_SPDIFIN_handle
//{
//	ADEV_CFG		adev_cfg;
//	SPDIFIN_CFG		spdifin_cfg;
//}
//ADEV_SPDIFIN_HANDLE, *PADEV_SPDIFIN_HANDLE;
typedef void (*silan_dma_cb)(int event_id, void *priv);
typedef struct {
	silan_dma_cb cb;
	void *priv;
}dma_cb_t;
void silan_dma_cb_reg(silan_dma_cb cb,void *priv);

int32_t silan_adev_i2s_submit(ADEV_I2S_HANDLE *pdev);
int32_t silan_adev_i2s_submit_ex(ADEV_I2S_HANDLE *pdev, void *cb);
void silan_adev_init(void);
ADEV_I2S_HANDLE *silan_adev_i2s_alloc(I2S_ID i2s);
int32_t silan_adev_i2s_open(ADEV_I2S_HANDLE *pdev);
int32_t silan_adev_i2s_set_rate(ADEV_I2S_HANDLE *pdev, int rate);
int32_t silan_adev_i2s_start(ADEV_I2S_HANDLE *pdev);
int32_t silan_adev_i2s_stop(ADEV_I2S_HANDLE *pdev);
int silan_adev_pre_write(void *pdev, char **write_p, int bytes);
int silan_adev_get_write_size(void *pdev);
int silan_adev_get_buffer_size(void *pdev);
int32_t silan_adev_get_read_size(void *pdev);
int silan_adev_pre_read(void *pdev, char **read_p, int bytes);
int silan_adev_read(void *pdev, int bytes);
int silan_adev_write(void *pdev, int bytes);
int32_t silan_adev_i2s_set_gain(ADEV_I2S_HANDLE *pdev, int32_t l_gain, int32_t r_gain);
int32_t silan_adev_i2s_revoke(ADEV_I2S_HANDLE *pdev);

#endif

