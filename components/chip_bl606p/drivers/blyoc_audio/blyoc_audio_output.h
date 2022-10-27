/*
 * blyoc_audio.h
 *
 */

#ifndef COMPONENTS_CHIP_BL606_DRIVERS_BLYOC_AUDIO_BLYOC_AUDIO_OUTPUT_H_
#define COMPONENTS_CHIP_BL606_DRIVERS_BLYOC_AUDIO_BLYOC_AUDIO_OUTPUT_H_
#include "blyoc_audio.h"

#define BLYOC_AUDIO_BASE                     AUDIO_BASE
#define BLYOC_AUDIO_IRQn                     AUDIO_IRQn
#define AUDIO_DAC_MAX                        2

#define AUDIO_TX_FIFO_ADDR                   AUDIO_BASE + 0x94
#define AUDIO_TX_PINGPONG_BUFFER_SIZE        (2400)
#define AUDIO_DEFAULT_TX_DRQ_THR              16

/*audio dma play configuration */
#define AUDIO_TX_DEFAULT_DMA_ID               DMA0_ID
#define AUDIO_TX_DEFAULT_DMA_CHANNEL          DMA_CH0

/*audio play default seting */
#define AUDIO_TX_DEFAULT_SAMPLE               BLYOC_AUDIO_RATE_16K
#define AUDIO_TX_DEFAULT_BIT_WIDTH            BLYOC_AUDIO_BIT_WIDTH_16   // AUDIO_BIT_WIDTH_16
#define AUDIO_TX_DEFAULT_INITIAL_VOLUME       0x0                       // 0xFFE0
#define AUDIO_TX_DEFAULT_CHANNEL_NUM          BLYOC_AUDIO_STEREO_CHANNEL// AUDIO_STEREO_CHANNEL      // AUDIO_LEFT_CHANNEL
/* audio dma max lth  */
#define AUDIO_DMA_TX_MAX_LTH                 (4 * 1024)


#endif /* COMPONENTS_CHIP_BL606_DRIVERS_BLYOC_AUDIO_BLYOC_AUDIO_H_ */
