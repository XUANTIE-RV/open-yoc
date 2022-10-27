/*
 * blyoc_audio_input.h
 *
 */

#ifndef COMPONENTS_CHIP_BL606_DRIVERS_BLYOC_AUDIO_BLYOC_AUDIO_INPUT_H_
#define COMPONENTS_CHIP_BL606_DRIVERS_BLYOC_AUDIO_BLYOC_AUDIO_INPUT_H_
#include "blyoc_audio.h"

#define audio_input_printk //printf

#define BLYOC_AUDIO_BASE                        AUDIO_BASE
#define BLYOC_AUDIO_IRQn                        AUDIO_IRQn
#define AUDIO_ADC_MAX                           4

#define AUDIO_RX_FIFO_ADDR                     BLYOC_AUDIO_BASE + 0x88
#define AUDIO_RX_PINGPONG_BUFFER_SIZE          (4800)
#define AUDIO_DEFAULT_RX_DRQ_THR                16

/*audio dma recording configuration */
#define BLYOC_AUDIO_RX_DEFAULT_DMA_ID           DMA0_ID                     // dma id
#define BLYOC_AUDIO_RX_DEFAULT_DMA_CHANNEL      DMA_CH1                     // dma ch id

/*audio recording default seting */
#define AUDIO_RECORDING_DEFAULT_SAMPLE          BLYOC_AUDIO_RATE_16K
#define AUDIO_RX_DEFAULT_CHANNEL_NUM            BLYOC_AUDIO_THREE_CHANNEL     // AUDIO_LEFT_CHANNEL
#define AUDIO_RX_DEFAULT_INITIAL_VOLUME         0//5// 17                     // 默认音量
#define AUDIO_RX_DEFAULT_BIT_WIDTH              BLYOC_AUDIO_BIT_WIDTH_16       // AUDIO_BIT_WIDTH_16
/* audio dma max lth  */
#define AUDIO_DMA_RX_MAX_LTH                   (4 * 1024)


#endif /* COMPONENTS_CHIP_BL606_DRIVERS_BLYOC_AUDIO_BLYOC_AUDIO_INPUT_H_ */
