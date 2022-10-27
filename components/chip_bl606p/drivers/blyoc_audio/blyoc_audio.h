/*
 * blyoc_audio.h
 *
 */

#ifndef COMPONENTS_CHIP_BL606_DRIVERS_BLYOC_AUDIO_BLYOC_AUDIO_H_
#define COMPONENTS_CHIP_BL606_DRIVERS_BLYOC_AUDIO_BLYOC_AUDIO_H_

/**
 *  @brief Audio Channel Sel
 */
typedef enum {
    AUDIO_LEFT_CHANNEL   = 1, /*!< select mono mode left only */
    AUDIO_RIGHT_CHANNEL  = 2,    /*!< select mono mode right only */
    AUDIO_STEREO_CHANNEL = 3,   /*!< select stereo */
    AUDIO_THREE_CHANNEL  = 7,
} Audio_Channel_Type;


/**
 *  @brief Audio Play/recording bit width
 */
typedef enum {
    AUDIO_BIT_WIDTH_16, /*!< 16 bit */
    AUDIO_BIT_WIDTH_24, /*!< 24 bit */
    AUDIO_BIT_WIDTH_32, /*!< 32 bit */
} Audio_BitWidth_Type;

/* audio sample  rate */
#define BLYOC_AUDIO_RATE_8K                 8000
#define BLYOC_AUDIO_RATE_16K                16000
#define BLYOC_AUDIO_RATE_24K                24000
#define BLYOC_AUDIO_RATE_32K                32000
#define BLYOC_AUDIO_RATE_48K                48000
#define BLYOC_AUDIO_RATE_96K                96000
#define BLYOC_AUDIO_RATE_192K               192000

/* audio bit width */
#define BLYOC_AUDIO_BIT_WIDTH_16            16
#define BLYOC_AUDIO_BIT_WIDTH_24            24
#define BLYOC_AUDIO_BIT_WIDTH_32            32
/* audio track */
#define BLYOC_AUDIO_LEFT_CHANNEL            1
#define BLYOC_AUDIO_STEREO_CHANNEL          2
#define BLYOC_AUDIO_THREE_CHANNEL           3

/* output function */
csi_error_t blyoc_audio_output_init(csi_codec_t *codec);
csi_error_t blyoc_audio_output_open(csi_codec_t *codec, csi_codec_output_t *codec_output_ch, uint32_t ch_idx);
csi_error_t blyoc_audio_output_cfg(csi_codec_output_t *codec_output_ch, csi_codec_output_config_t *config);
csi_error_t blyoc_audio_tx_dma_link(csi_codec_output_t *codec_output_ch,  csi_dma_ch_t *dma);
void blyoc_audio_tx_start(csi_codec_output_t *codec_output_ch, uint8_t dma_id, uint8_t channel);
uint32_t blyoc_audio_output_write(csi_codec_output_t *codec_output_ch, const void *data, uint32_t size);
void blyoc_audio_tx_stop(csi_codec_output_t * codec_output_ch, uint8_t dma_id, uint8_t channel);
void blyoc_audio_output_close(csi_codec_output_t *codec_output_ch);

uint32_t blyoc_audio_output_set_mute(csi_codec_output_t *ch, bool enable);
uint32_t blyoc_audio_output_set_mix_gain(csi_codec_output_t *ch, uint32_t enable, uint32_t val);
uint32_t blyoc_audio_output_digital_gain(csi_codec_output_t *ch,  uint32_t enable, uint32_t val);
uint32_t blyoc_audio_output_analog_gain(csi_codec_output_t *ch, uint32_t val);

void blyoc_audio_output_pause(csi_codec_output_t *ch);
void blyoc_audio_output_resume(csi_codec_output_t *ch);
uint32_t blyoc_audio_output_buffer_avail(csi_codec_output_t *ch);
uint32_t blyoc_audio_output_buffer_remain(csi_codec_output_t *ch);
void blyoc_audio_output_buffer_reset(csi_codec_output_t *ch);
/* input function */
csi_error_t blyoc_audio_input_init(csi_codec_t *codec);
csi_error_t blyoc_audio_input_open(csi_codec_t *codec, csi_codec_input_t *codec_input_ch, uint32_t ch_idx);
csi_error_t blyoc_audio_input_cfg(csi_codec_input_t *codec_input_ch, csi_codec_input_config_t *config);
csi_error_t blyoc_audio_rx_dma_link(csi_codec_input_t *codec_input_ch, csi_dma_ch_t *dma);
csi_error_t blyoc_audio_rx_start(csi_codec_input_t * codec_input_ch, uint8_t dma_id, uint8_t channel);
uint32_t blyoc_audio_input_read(csi_codec_input_t *codec_input_ch, void *data, uint32_t size);
csi_error_t blyoc_audio_rx_stop(csi_codec_input_t *codec_input_ch, uint8_t dma_id,uint8_t channel);
void blyoc_audio_input_close(csi_codec_input_t *codec_input_ch);

uint32_t blyoc_audio_input_set_mute(csi_codec_input_t *ch, bool enable);
uint32_t blyoc_audio_input_set_mix_gain(csi_codec_input_t *ch, uint32_t enable, uint32_t val);
uint32_t blyoc_audio_input_digital_gain(csi_codec_input_t *ch,  uint32_t enable, uint32_t val);
uint32_t blyoc_audio_input_analog_gain(csi_codec_input_t *ch, uint32_t val);

uint32_t blyoc_audio_input_buffer_avail(csi_codec_input_t *ch);
uint32_t blyoc_audio_input_buffer_remain(csi_codec_input_t *ch);
void blyoc_audio_input_buffer_reset(csi_codec_input_t *ch);

#endif /* COMPONENTS_CHIP_BL606_DRIVERS_BLYOC_AUDIO_BLYOC_AUDIO_H_ */
