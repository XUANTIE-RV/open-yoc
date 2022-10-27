#ifndef COMPONENTS_CHIP_BL606_INCLUDE_BLYOC_CODEC_H_
#define COMPONENTS_CHIP_BL606_INCLUDE_BLYOC_CODEC_H_
//#include <soc.h>
/**
 *  @brief Audio Channel Sel
 */
typedef enum {
    LEFT_CHANNEL = 1, /*!< select mono mode left only */
    RIGHT_CHANNEL,    /*!< select mono mode right only */
    STEREO_CHANNEL,   /*!< select stereo */
    THREE_CHANNEL = 7,    /*!< three channel */
} Aupdm_Channel_Type;

/**
 *  @brief Audio Play/recording bit width
 */
typedef enum {
    AUPDM_BIT_WIDTH_16, /*!< 16 bit */
    AUPDM_BIT_WIDTH_24, /*!< 24 bit */
    AUPDM_BIT_WIDTH_32, /*!< 32 bit */
} Aupdm_BitWidth_Type;


/* audio base message */
#define BLYOC_AUPDM_BASE                        PDM0_BASE
#define BLYOC_AUPDM_IRQn                        AUPDM_IRQn
#define BLYOC_AUPDM_RX_FIFO_ADDR                BLYOC_AUPDM_BASE + 0xC88

/*audio recording configuration */
#define BLYOC_AUPDM_RX_DEFAULT_DMA_ID           DMA0_ID                // dma id
#define BLYOC_AUPDM_RX_DEFAULT_DMA_CHANNEL      DMA_CH1                // dma ch id

/*audio recording default seting */
#define AUDIO_RECORDING_DEFAULT_SAMPLE    16 * 1000
#define AUDIO_RX_DEFAULT_CHANNEL_NUM      STEREO_CHANNEL
#define AUDIO_RX_DEFAULT_INITIAL_VOLUME   17                     // 默认音量
#define AUDIO_RX_DEFAULT_BIT_WIDTH        AUPDM_BIT_WIDTH_16

/*audio recv addr */
#define BLYOC_FILE_STORE_ADDR                            0x8E000000
#define AUDIO_RX_PINGPONG_BUFFER_SIZE              (2 * 1024)
#define BLYOC_FILE_SIZE                                  48 * 10000

/* DRQ  Threshold */
#define AUDIO_DEFAULT_TX_DRQ_THR                8
#define AUDIO_DEFAULT_RX_DRQ_THR                8
/* aupdm sample  rate */
#define BLYOC_AUPDM_RATE_8K                 8000
#define BLYOC_AUPDM_RATE_16K                16000
#define BLYOC_AUPDM_RATE_24K                24000
#define BLYOC_AUPDM_RATE_32K                32000
#define BLYOC_AUPDM_RATE_48K                48000
#define BLYOC_AUPDM_RATE_96K                96000

/* aupdm bit width */
#define BLYOC_AUPDM_BIT_WIDTH_16                16
#define BLYOC_AUPDM_BIT_WIDTH_24                24
#define BLYOC_AUPDM_BIT_WIDTH_32                32


/*                            function                              */
csi_error_t blyoc_aupdm_init(csi_codec_t *codec);

csi_error_t blyoc_aupdm_input_open(csi_codec_t *codec, csi_codec_input_t *codec_input_ch, uint32_t ch_idx);
csi_error_t blyoc_aupdm_rx_dma_link(csi_codec_t *codec);
csi_error_t blyoc_aupdm_input_cfg(csi_codec_input_t *codec_input_ch, csi_codec_input_config_t *config);
csi_error_t blyoc_aupdm_rx_start(csi_codec_input_t * codec_input_ch, uint8_t dma_id, uint8_t channel);
uint32_t blyoc_aupdm_input_read(csi_codec_input_t *codec_input_ch, void *data, uint32_t size);
csi_error_t blyoc_aupdm_rx_stop(csi_codec_input_t *codec_input_ch, uint8_t dma_id,uint8_t channel);
void blyoc_aupdm_input_close(csi_codec_input_t *codec_input_ch);
#endif /* COMPONENTS_CHIP_BL606_INCLUDE_BLYOC_CODEC_H_ */


