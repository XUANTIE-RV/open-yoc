/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <sys_clk.h>
#include <drv_codec.h>
#include <drv/irq.h>
#include <drv/codec.h>

extern const uint16_t codec_tx_hs_num[];
extern const uint16_t codec_rx_hs_num[];

/**
  \brief  Init the codec according to the specified
  \param[in]   codec codec handle to operate.
  \param[in]   idx   codec interface idx
  \return      error code
*/
csi_error_t csi_codec_init(csi_codec_t *codec, uint32_t idx)
{
    CSI_PARAM_CHK(codec, CSI_ERROR);
    csi_error_t ret = CSI_OK;
    ret = target_get(DEV_WJ_CODEC_TAG, idx, &codec->dev);
    codec->priv = (void *)idx;

    if (codec_init(idx) == -1) {
        ret = CSI_ERROR;
    }

    return ret;
}

/**
  \brief       codec uninit
  \param[in]   codec codec handle to operate.
  \return      none
*/
void csi_codec_uninit(csi_codec_t *codec)
{
    codec_uninit((uint32_t)codec->priv);
}

/**
  \brief  Open a codec output channel
  \param[in]   codec codec handle to operate.
  \param[in]   ch codec output channel handle.
  \param[in]   ch_idx codec output channel idx.
  \return      error code
*/
csi_error_t csi_codec_output_open(csi_codec_t *codec, csi_codec_output_t *ch, uint32_t ch_idx)
{
    csi_error_t ret = CSI_OK;
    ch->ch_idx = ch_idx;
    return ret;
}

/**
  \brief  Config codec output channel
  \param[in]   ch codec output channel handle.
  \param[in]   config codec channel prarm.
  \return      error code
*/
static void __codec_output_event_cb(int idx, codec_event_t event, void *arg)
{
    csi_codec_output_t *ch = (csi_codec_output_t *)arg;
    if (event == CODEC_EVENT_VAD_TRIGGER) {
        return;
    }

    if (event == CODEC_EVENT_TRANSFER_ERROR) {
        event = CODEC_EVENT_ERROR;
    }

    if (ch->callback != NULL) {
        ch->callback(ch, event, ch->arg);
    }
}
csi_error_t csi_codec_output_config(csi_codec_output_t *ch, csi_codec_output_config_t *config)
{
    CSI_PARAM_CHK(ch, CSI_ERROR);
    CSI_PARAM_CHK(config, CSI_ERROR);
    codec_output_t  ch_data = {0};
    codec_output_config_t ch_config = {0};
    csi_error_t ret = CSI_OK;

    ch_data.ch_idx = ch->ch_idx;
    ch_data.codec_idx = 0;
    ch_data.cb = __codec_output_event_cb;
    ch_data.cb_arg = ch;
    ch->ring_buf->buffer = config->buffer;
    ch->ring_buf->size  = config->buffer_size;
    ch->period = config->period;

    ch_data.buf =  ch->ring_buf->buffer;
    ch_data.buf_size = ch->ring_buf->size;
    ch_data.period = ch->period;
    ch_data.priv = ch->priv;

    ch_config.sample_rate = config->sample_rate;
    ch_config.bit_width = config->bit_width;
    ch_config.mono_mode_en = config->sound_channel_num == 1 ? 1 : 0;

    if (-1 == codec_output_open(&ch_data)) {
        ret = CSI_ERROR;
    }

    if (-1 == codec_output_config(&ch_data, &ch_config)) {
        ret = CSI_ERROR;
    }

    return ret;
}

/**
  \brief       Attach the callback handler to codec output
  \param[in]   ch codec output channel handle.
  \param[in]   cb    callback function
  \param[in]   arg   user can define it by himself as callback's param
  \return      error code
*/
csi_error_t csi_codec_output_attach_callback(csi_codec_output_t *ch, void *callback, void *arg)
{
    CSI_PARAM_CHK(ch, CSI_ERROR);
    csi_error_t ret = CSI_OK;
    ch->callback = callback;
    ch->arg = arg;
    return ret;
}

/**
  \brief       detach the callback handler
  \param[in]   ch codec output channel handle.
  \return      none
*/
void csi_codec_output_detach_callback(csi_codec_output_t *ch)
{
    CSI_PARAM_CHK_NORETVAL(ch);
    ch->callback = NULL;
    ch->arg = NULL;
}

/**
  \brief  Close a CODEC output channel
  \param[in]   ch codec output channel handle.
  \return      error code
*/
void csi_codec_output_close(csi_codec_output_t *ch)
{
    codec_output_t  ch_data = {0};

    ch_data.codec_idx = 0;
    ch_data.ch_idx = ch->ch_idx;
    ch_data.buf = ch->ring_buf->buffer;
    ch_data.buf_size = ch->ring_buf->size;
    ch_data.period = ch->period;
    ch_data.priv = ch->priv;

    codec_output_close(&ch_data);
}

/**
  \brief       link DMA channel to codec output channel
  \param[in]   ch codec output channel handle.
  \param[in]   dma dma channel info.
  \return      error code
*/
csi_error_t csi_codec_output_link_dma(csi_codec_output_t *ch, csi_dma_ch_t *dma)
{
    CSI_PARAM_CHK(ch, CSI_ERROR);
    csi_error_t ret = CSI_OK;
    CSI_PARAM_CHK(ch->codec, CSI_ERROR);
    return ret;
}

/**
  \brief       write an amount of data to cache in blocking mode.
  \param[in]   ch    the codec output channel
  \param[in]   data  send data.
  \param[in]   size  receive data size.
  \return      The num of data witch is send successful
*/
uint32_t csi_codec_output_write(csi_codec_output_t *ch, const void *data, uint32_t size)
{
    CSI_PARAM_CHK(ch, CSI_ERROR);
    codec_output_t  ch_data = {0};
    uint32_t ret = 0;

    ch_data.codec_idx = 0;
    ch_data.ch_idx = ch->ch_idx;
    ch_data.buf = ch->ring_buf->buffer;
    ch_data.buf_size = ch->ring_buf->size;
    ch_data.period = ch->period;
    ch_data.priv = ch->priv;

    ret = codec_output_write(&ch_data, (uint8_t *)data, size);
    return ret;
}

/**
  \brief Write data to the cache.
  \With asynchronous sending,
  \the data is first written to the cache and then output through the codec interface.
  \This function does not block, and the return value is the number
  \of data that was successfully written to the cache.
  \param[in]   ch    the codec output channel
  \param[in]   data  send data.
  \param[in]   size  receive data size.
  \return      The data size that write to cache
*/
uint32_t csi_codec_output_write_async(csi_codec_output_t *ch, const void *data, uint32_t size)
{
    CSI_PARAM_CHK(ch, CSI_ERROR);
    codec_output_t  ch_data = {0};
    uint32_t ret = 0;

    ch_data.codec_idx = 0;
    ch_data.ch_idx = ch->ch_idx;
    ch_data.buf = ch->ring_buf->buffer;
    ch_data.buf_size = ch->ring_buf->size;
    ch_data.period = ch->period;
    ch_data.priv = ch->priv;

    ret = codec_output_write(&ch_data, (uint8_t *)data, size);
    return ret;
}

/**
  \brief       Start sending data from the cache
  \param[in]   ch codec output channel handle.
  \return      error code
*/
csi_error_t csi_codec_output_start(csi_codec_output_t *ch)
{
    CSI_PARAM_CHK(ch, CSI_ERROR);
    codec_output_t  ch_data = {0};
    csi_error_t ret = CSI_OK;

    ch_data.codec_idx = 0;
    ch_data.ch_idx = ch->ch_idx;
    ch_data.buf = ch->ring_buf->buffer;
    ch_data.buf_size = ch->ring_buf->size;
    ch_data.period = ch->period;
    ch_data.priv = ch->priv;

    if (-1 == codec_output_start(&ch_data)) {
        ret = CSI_ERROR;
    }

    return ret;
}

/**
  \brief       Stop sending data from the cache
  \param[in]   ch codec output channel handle.
  \return      error code
*/
void csi_codec_output_stop(csi_codec_output_t *ch)
{
    codec_output_t  ch_data = {0};

    ch_data.codec_idx = 0;
    ch_data.ch_idx = ch->ch_idx;
    ch_data.buf = ch->ring_buf->buffer;
    ch_data.buf_size = ch->ring_buf->size;
    ch_data.period = ch->period;
    ch_data.priv = ch->priv;

    codec_output_stop(&ch_data);
}

/**
  \brief       Pause sending data from the cache
  \param[in]   ch codec output channel handle.
  \return      error code
*/
csi_error_t csi_codec_output_pause(csi_codec_output_t *ch)
{
    CSI_PARAM_CHK(ch, CSI_ERROR);
    codec_output_t  ch_data = {0};
    csi_error_t ret = CSI_OK;

    ch_data.codec_idx = 0;
    ch_data.ch_idx = ch->ch_idx;
    ch_data.buf = ch->ring_buf->buffer;
    ch_data.buf_size = ch->ring_buf->size;
    ch_data.period = ch->period;
    ch_data.priv = ch->priv;

    if (-1 == codec_output_pause(&ch_data)) {
        ret = CSI_ERROR;
    }

    return ret;
}

/**
  \brief       Resume sending data from the cache
  \param[in]   ch codec output channel handle.
  \return      error code
*/
csi_error_t csi_codec_output_resume(csi_codec_output_t *ch)
{
    CSI_PARAM_CHK(ch, CSI_ERROR);
    codec_output_t  ch_data = {0};
    csi_error_t ret = CSI_OK;
    // ch_data.codec_idx = (int32_t)codec->priv;
    ch_data.codec_idx = 0;
    ch_data.ch_idx = ch->ch_idx;
    ch_data.buf = ch->ring_buf->buffer;
    ch_data.buf_size = ch->ring_buf->size;
    ch_data.period = ch->period;
    ch_data.priv = ch->priv;

    if (-1 == codec_output_resume(&ch_data)) {
        ret = CSI_ERROR;
    }

    return ret;
}

/**
  \brief  Get cache free space
  \param[in]   ch codec output channel handle.
  \return buffer free space (bytes)
*/
uint32_t csi_codec_output_buffer_avail(csi_codec_output_t *ch)
{
    CSI_PARAM_CHK(ch, CSI_ERROR);
    codec_output_t  ch_data = {0};
    uint32_t ret = 0;

    ch_data.codec_idx = 0;
    ch_data.ch_idx = ch->ch_idx;
    ch_data.buf = ch->ring_buf->buffer;
    ch_data.buf_size = ch->ring_buf->size;
    ch_data.period = ch->period;
    ch_data.priv = ch->priv;

    ret = codec_output_buf_avail(&ch_data);

    return ret;
}

/**
  \brief  Get cache free space
  \param[in]   ch codec input channel handle.
  \return buffer free space (bytes)
*/
uint32_t csi_codec_input_buffer_avail(csi_codec_input_t *ch)
{
    CSI_PARAM_CHK(ch, CSI_ERROR);
    codec_input_t  ch_data = {0};
    volatile uint32_t ret = 0;

    ch_data.codec_idx = 0;
    ch_data.ch_idx = ch->ch_idx;
    ch_data.buf = ch->ring_buf->buffer;
    ch_data.buf_size = ch->ring_buf->size;
    ch_data.period = ch->period;
    ch_data.priv = ch->priv;

    ret = codec_input_buf_avail(&ch_data);

    return ret;
}


/**
  \brief  Reset the buf, discard all data in the cache
  \param[in]   ch codec output channel handle.
  \return      error code
*/
csi_error_t csi_codec_output_buffer_reset(csi_codec_output_t *ch)
{
    CSI_PARAM_CHK(ch, CSI_ERROR);
    codec_output_t  ch_data = {0};
    csi_error_t ret = CSI_OK;

    ch_data.codec_idx = 0;
    ch_data.ch_idx = ch->ch_idx;
    ch_data.buf = ch->ring_buf->buffer;
    ch_data.buf_size = ch->ring_buf->size;
    ch_data.period = ch->period;
    ch_data.priv = ch->priv;

    codec_output_buf_reset(&ch_data);

    return ret;
}

/**
  \brief       mute codec ouput channel
  \param[in]   ch codec output channel handle.
  \param[in]   en true codec mute.
  \return      error code
*/
csi_error_t csi_codec_output_mute(csi_codec_output_t *ch, bool enable)
{
    CSI_PARAM_CHK(ch, CSI_ERROR);
    codec_output_t  ch_data = {0};
    csi_error_t ret = CSI_OK;

    ch_data.codec_idx = 0;
    ch_data.ch_idx = ch->ch_idx;
    ch_data.buf = ch->ring_buf->buffer;
    ch_data.buf_size = ch->ring_buf->size;
    ch_data.period = ch->period;
    ch_data.priv = ch->priv;

    codec_output_mute(&ch_data, enable);
    return ret;
}

/**
  \brief       Set codec ouput channel digital gain.
  \param[in]   ch codec output channel handle.
  \param[in]   val gain val.
  \return      error code
*/
csi_error_t csi_codec_output_digital_gain(csi_codec_output_t *ch, uint32_t val)
{
    CSI_PARAM_CHK(ch, CSI_ERROR);
    codec_output_t  ch_data = {0};
    csi_error_t ret = CSI_OK;

    ch_data.codec_idx = 0;
    ch_data.ch_idx = ch->ch_idx;
    ch_data.buf = ch->ring_buf->buffer;
    ch_data.buf_size = ch->ring_buf->size;
    ch_data.period = ch->period;
    ch_data.priv = ch->priv;

    codec_output_set_digital_right_gain(&ch_data, val);
    codec_output_set_digital_left_gain(&ch_data, val);
    return ret;
}

/**
  \brief       Set codec ouput channel analog gain.
  \param[in]   ch codec output channel handle.
  \param[in]   val gain val.
  \return      error code
*/
csi_error_t csi_codec_output_analog_gain(csi_codec_output_t *ch, uint32_t val)
{
    CSI_PARAM_CHK(ch, CSI_ERROR);
    codec_output_t  ch_data = {0};
    csi_error_t ret = CSI_OK;

    ch_data.codec_idx = 0;
    ch_data.ch_idx = ch->ch_idx;
    ch_data.buf = ch->ring_buf->buffer;
    ch_data.buf_size = ch->ring_buf->size;
    ch_data.period = ch->period;
    ch_data.priv = ch->priv;

    codec_output_set_analog_right_gain(&ch_data, val);
    codec_output_set_analog_left_gain(&ch_data, val);
    return ret;
}

/**
  \brief       Set codec ouput channel mix gain.
  \param[in]   ch codec output channel handle.
  \param[in]   val gain val.
  \return      error code
*/
csi_error_t csi_codec_output_mix_gain(csi_codec_output_t *ch, uint32_t val)
{
    CSI_PARAM_CHK(ch, CSI_ERROR);
    codec_output_t  ch_data = {0};
    csi_error_t ret = CSI_OK;

    ch_data.codec_idx = 0;
    ch_data.ch_idx = ch->ch_idx;
    ch_data.buf = ch->ring_buf->buffer;
    ch_data.buf_size = ch->ring_buf->size;
    ch_data.period = ch->period;
    ch_data.priv = ch->priv;

    codec_output_set_mixer_right_gain(&ch_data, val);
    codec_output_set_mixer_left_gain(&ch_data, val);
    return ret;
}

/**
  \brief       Get codec output channel state.
  \param[in]   ch codec output channel handle.
  \param[out]  state channel state.
  \return      channel state
*/
csi_error_t csi_codec_output_get_state(csi_codec_output_t *ch, csi_state_t *state)
{
    return CSI_OK;
}

/**
  \brief  Open a CODEC input channel
  \param[in]   codec codec handle to operate.
  \param[in]   ch codec input channel handle.
  \param[in]   ch_idx codec input channel idx.
  \return      error code
*/
csi_error_t csi_codec_input_open(csi_codec_t *codec, csi_codec_input_t *ch, uint32_t ch_idx)
{
    ch->ch_idx = ch_idx;
    return CSI_OK;
}

/**
  \brief  Config codec input channel
  \param[in]   ch codec input channel handle.
  \param[in]   config codec channel prarm.
  \return      error code
*/

static void __codec_input_event_cb(int idx, codec_event_t event, void *arg)
{
    csi_codec_input_t *ch = (csi_codec_input_t *)arg;
    if (event == CODEC_EVENT_VAD_TRIGGER) {
        return;
    }

    if (event == CODEC_EVENT_TRANSFER_ERROR) {
        event = CODEC_EVENT_ERROR;
    }

    if (ch->callback != NULL) {
        ch->callback(ch, event, ch->arg);
    }
}

csi_error_t csi_codec_input_config(csi_codec_input_t *ch, csi_codec_input_config_t *config)
{
    CSI_PARAM_CHK(ch, CSI_ERROR);
    CSI_PARAM_CHK(config, CSI_ERROR);
    codec_input_t  ch_data = {0};
    codec_input_config_t ch_config = {0};
    csi_error_t ret = CSI_OK;
    ch_data.codec_idx = 0;
    ch_data.ch_idx = ch->ch_idx;
    ch_data.cb = __codec_input_event_cb;
    ch_data.cb_arg = ch;

    ch->ring_buf->buffer = config->buffer;
    ch->ring_buf->size  = config->buffer_size;
    ch->period = config->period;

    ch_data.buf =  ch->ring_buf->buffer;
    ch_data.buf_size = ch->ring_buf->size;
    ch_data.period = ch->period;
    ch_data.priv = ch->priv;

    ch_config.sample_rate = config->sample_rate;
    ch_config.bit_width = config->bit_width;
    ch_config.channel_num = config->sound_channel_num;

    if (-1 == codec_input_open(&ch_data)) {
        ret = CSI_ERROR;
    }

    if (-1 == codec_input_config(&ch_data, &ch_config)) {
        ret = CSI_ERROR;
    }

    return ret;
}

/**
  \brief       Attach the callback handler to codec output
  \param[in]   ch codec input channel handle.
  \param[in]   cb    callback function
  \param[in]   arg   user can define it by himself as callback's param
  \return      error code
*/
csi_error_t csi_codec_input_attach_callback(csi_codec_input_t *ch, void *callback, void *arg)
{
    CSI_PARAM_CHK(ch, CSI_ERROR);
    csi_error_t ret = CSI_OK;
    ch->callback = callback;
    ch->arg = arg;
    return ret;
}

/**
  \brief       detach the callback handler
  \param[in]   ch codec input channel handle.
  \return      none
*/
void csi_codec_input_detach_callback(csi_codec_input_t *ch)
{
    CSI_PARAM_CHK_NORETVAL(ch);
    ch->callback = NULL;
    ch->arg = NULL;
}

/**
  \brief  Close a CODEC input channel
  \param[in]   ch codec input channel handle..
  \return      error code
*/
void csi_codec_input_close(csi_codec_input_t *ch)
{
    codec_input_t  ch_data = {0};

    ch_data.codec_idx = 0;
    ch_data.ch_idx = ch->ch_idx;
    ch_data.buf = ch->ring_buf->buffer;
    ch_data.buf_size = ch->ring_buf->size;
    ch_data.period = ch->period;
    ch_data.priv = ch->priv;

    codec_input_close(&ch_data);
}

/**
  \brief       link DMA channel to codec input channel
  \param[in]   ch codec input channel handle.
  \param[in]   dma dma channel info.
  \return      error code
*/
csi_error_t csi_codec_input_link_dma(csi_codec_input_t *ch, csi_dma_ch_t *dma)
{
    return CSI_OK;
}

/**
  \brief  read an amount of data in blocking mode.
  \param[in]   ch codec input channel handle.
  \param[in]   data  save receive data.
  \param[in]   size  receive data size.
  \return      The size of data receive successfully
*/
uint32_t csi_codec_input_read(csi_codec_input_t *ch, void *data, uint32_t size)
{
    CSI_PARAM_CHK(ch, CSI_ERROR);
    codec_input_t  ch_data = {0};
    uint32_t ret = 0;

    ch_data.codec_idx = 0;
    ch_data.ch_idx = ch->ch_idx;
    ch_data.buf = ch->ring_buf->buffer;
    ch_data.buf_size = ch->ring_buf->size;
    ch_data.period = ch->period;
    ch_data.priv = ch->priv;

    ret = codec_input_read(&ch_data, data, size);

    return ret;

}

/**
  \brief Read data from the cache.
  \Using asynchronous receive.
  \This function read data from the cache, returns the number of successful reads,
  \and returns 0 if there is no data in the cache.
  \param[in]   ch codec input channel handle.
  \param[in]   data  the buf save receive data.
  \param[in]   size  receive data size.
  \return      The size of data read successfully
*/
uint32_t csi_codec_input_read_async(csi_codec_input_t *ch, void *data, uint32_t size)
{
    CSI_PARAM_CHK(ch, CSI_ERROR);
    codec_input_t  ch_data = {0};
    uint32_t ret = 0;
    ch_data.codec_idx = 0;
    ch_data.ch_idx = ch->ch_idx;
    ch_data.buf = ch->ring_buf->buffer;
    ch_data.buf_size = ch->ring_buf->size;
    ch_data.period = ch->period;
    ch_data.priv = ch->priv;

    ret = codec_input_read(&ch_data, data, size);
    return ret;
}

/**
  \brief       Start receive data, writes the recording data to the cache
  \param[in]   ch codec input channel handle.
  \return      error code
*/
csi_error_t csi_codec_input_start(csi_codec_input_t *ch)
{
    CSI_PARAM_CHK(ch, CSI_ERROR);
    codec_input_t  ch_data = {0};
    csi_error_t ret = CSI_OK;
    ch_data.codec_idx = 0;
    ch_data.ch_idx = ch->ch_idx;
    ch_data.buf = ch->ring_buf->buffer;
    ch_data.buf_size = ch->ring_buf->size;
    ch_data.period = ch->period;
    ch_data.priv = ch->priv;

    if (-1 == codec_input_start(&ch_data)) {
        ret = CSI_ERROR;
    }

    return ret;
}
/**
  \brief       Stop receive data.
  \param[in]   ch codec input channel handle.
  \return      error code
*/
void csi_codec_input_stop(csi_codec_input_t *ch)
{
    codec_input_t  ch_data = {0};
    ch_data.codec_idx = 0;
    ch_data.ch_idx = ch->ch_idx;
    ch_data.buf = ch->ring_buf->buffer;
    ch_data.buf_size = ch->ring_buf->size;
    ch_data.period = ch->period;
    ch_data.priv = ch->priv;

    codec_input_stop(&ch_data);

}

/**
  \brief       mute codec input channel
  \param[in]   ch codec input channel handle.
  \param[in]   en true codec mute.
  \return      error code
*/
csi_error_t csi_codec_input_mute(csi_codec_input_t *ch, bool en)
{
    CSI_PARAM_CHK(ch, CSI_ERROR);
    codec_input_t  ch_data = {0};
    csi_error_t ret = CSI_OK;

    ch_data.codec_idx = 0;
    ch_data.ch_idx = ch->ch_idx;
    ch_data.buf = ch->ring_buf->buffer;
    ch_data.buf_size = ch->ring_buf->size;
    ch_data.period = ch->period;
    ch_data.priv = ch->priv;

    codec_input_mute(&ch_data, en);
    return ret;
}

/**
  \brief       Set codec input channel digital gain.
  \param[in]   ch codec input channel handle.
  \param[in]   val gain val.
  \return      error code
*/
csi_error_t csi_codec_input_digital_gain(csi_codec_input_t *ch, uint32_t val)
{
    CSI_PARAM_CHK(ch, CSI_ERROR);
    codec_input_t  ch_data = {0};
    csi_error_t ret = CSI_OK;
    ch_data.codec_idx = 0;
    ch_data.ch_idx = ch->ch_idx;
    ch_data.buf = ch->ring_buf->buffer;
    ch_data.buf_size = ch->ring_buf->size;
    ch_data.period = ch->period;
    ch_data.priv = ch->priv;

    codec_input_set_digital_gain(&ch_data, val);

    return ret;
}

/**
  \brief       Set codec input channel analog gain.
  \param[in]   ch codec input channel handle.
  \param[in]   val gain val.
  \return      error code
*/
csi_error_t csi_codec_input_analog_gain(csi_codec_input_t *ch, uint32_t val)
{
    codec_input_t  ch_data = {0};
    csi_error_t ret = CSI_OK;
    ch_data.codec_idx = 0;
    ch_data.ch_idx = ch->ch_idx;
    ch_data.buf = ch->ring_buf->buffer;
    ch_data.buf_size = ch->ring_buf->size;
    ch_data.period = ch->period;
    ch_data.priv = ch->priv;

    codec_input_set_analog_gain(&ch_data, val);

    return ret;
}

/**
  \brief       Set codec input channel mix gain.
  \param[in]   ch codec input channel handle.
  \param[in]   val gain val.
  \return      error code
*/
csi_error_t csi_codec_input_mix_gain(csi_codec_input_t *ch, uint32_t val)
{
    codec_input_t  ch_data = {0};
    csi_error_t ret = CSI_OK;

    ch_data.codec_idx = 0;
    ch_data.ch_idx = ch->ch_idx;
    ch_data.buf = ch->ring_buf->buffer;
    ch_data.buf_size = ch->ring_buf->size;
    ch_data.period = ch->period;
    ch_data.priv = ch->priv;

    codec_input_set_mixer_gain(&ch_data, val);

    return ret;
}

/**
  \brief       Get codec input channel state.
  \param[in]   ch codec input channel handle.
  \param[out]  state channel state.
  \return      channel state
*/
csi_error_t csi_codec_input_get_state(csi_codec_input_t *ch, csi_state_t *state)
{
    return CSI_OK;
}

#ifdef CONFIG_PM
csi_error_t pangu_codec_pm_action(csi_dev_t *dev, csi_pm_dev_action_t action)
{
    CSI_PARAM_CHK(dev, CSI_ERROR);

    csi_error_t ret = CSI_OK;

    switch (action) {
        case PM_DEV_SUSPEND:
            ck_codec_lpm(dev->idx,action);
            break;

        case PM_DEV_RESUME:
            ck_codec_lpm(dev->idx,action);
            break;

        default:
            ret = CSI_ERROR;
            break;
    }

    return ret;
}

csi_error_t csi_codec_enable_pm(csi_codec_t *codec)
{
    return csi_pm_dev_register(&codec->dev, pangu_codec_pm_action, 10U, 0U);
}

void csi_codec_disable_pm(csi_codec_t *codec)
{
    csi_pm_dev_unregister(&codec->dev);
}
#endif



