/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */


/******************************************************************************
 * @file     silan_i2s_csi.c
 * @brief    CSI Source File for I2S Driver
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/
#include <csi_config.h>
#include <string.h>
#include <drv/i2s.h>
#include "silan_adev.h"
#include "silan_syscfg.h"
#include "silan_printf.h"
#include "silan_errno.h"

#define ERR_I2S(errno) (CSI_DRV_ERRNO_I2S_BASE | errno)
#define I2S_NULL_PARAM_CHK(para) HANDLE_PARAM_CHK(para, ERR_I2S(DRV_ERROR_PARAMETER))
#define I2S_BUSY_TIMEOUT    0x1000000

static const i2s_capabilities_t i2s_capabilities = {
    .protocol_user      = 0,    /* supports user defined Protocol */
    .protocol_i2s       = 1,    /* supports I2S Protocol */
    .protocol_justified = 1,    /* supports MSB/LSB justified Protocol */
    .protocol_pcm       = 1,    /* supports PCM Protocol */
    .mono_mode          = 0,    /* supports Mono mode */
    .full_duplex        = 0,    /* supports full duplex */
    .mclk_pin           = 0,    /* supports MCLK (Master Clock) pin */
    .event_frame_error  = 1,    /* supports Frame error event: ARM_SAI_EVENT_FRAME_ERROR */
};

extern int32_t target_i2s_init(int32_t idx, uint32_t *base, uint32_t *irq);
extern int32_t target_get_addr_space(uint32_t addr);
extern int32_t silan_adev_i2s_revoke(ADEV_I2S_HANDLE *pdev);

void silan_i2s_irqhandler(int32_t idx)
{
}

/**
  \brief       Initialize I2S Interface. 1. Initializes the resources needed for the I2S interface 2.registers event callback function
  \param[in]   idx must not exceed return value of csi_i2s_get_instance_count()
  \return      pointer to i2s instances
*/
i2s_handle_t csi_i2s_initialize(int32_t idx, i2s_event_cb_t cb_event, void *cb_arg)
{
    ADEV_I2S_HANDLE *handle;

    if (idx < 0 || idx > SILAN_I2S_ID_PCM) {
        return NULL;
    }

    silan_codec1_cclk_config(CLK_ON);
    silan_codec2_cclk_config(CLK_ON);

    silan_dmac_req_matrix_init();
    silan_dmac_init();
    silan_adev_init();

    handle = silan_adev_i2s_alloc((I2S_ID)idx);

    if (handle == NULL) {
        return NULL;
    }

    if ((idx == SILAN_I2S_ID_I1) || (idx == SILAN_I2S_ID_O1)) {
        handle->i2s_cfg.codec = CODEC_ID_1;
    } else if ((idx == SILAN_I2S_ID_I2) || (idx == SILAN_I2S_ID_O2)) {
        handle->i2s_cfg.codec = CODEC_ID_2;
    }  else if ((idx == SILAN_I2S_ID_I3) || (idx == SILAN_I2S_ID_PCM)) {
        handle->i2s_cfg.codec = CODEC_ID_EXT;
    }

    if (idx == SILAN_I2S_ID_I1) {
        handle->codec_cfg.adc_sel  = CODEC_ADSEL_AUX;
        handle->codec_cfg.adc_diff = CODEC_SINGLE;
    }

    handle->i2s_cfg.ch         = I2S_CH_20;
    handle->i2s_cfg.ws         = I2S_WS_32;

    return handle;
}

/**
  \brief       De-initialize I2S Interface. stops operation and releases the software resources used by the interface
  \param[in]   handle i2s handle to operate.
  \return      error code
*/
int32_t csi_i2s_uninitialize(i2s_handle_t handle)
{
    I2S_NULL_PARAM_CHK(handle);

    silan_adev_i2s_revoke((ADEV_I2S_HANDLE *)handle);
    return 0;
}

/**
  \brief       Get driver capabilities.
  \param[in]   idx i2s index
  \return      \ref i2s_capabilities_t
*/
i2s_capabilities_t csi_i2s_get_capabilities(int32_t idx)
{
    if (idx < 0 || idx > SILAN_I2S_ID_PCM) {
        i2s_capabilities_t ret;
        memset(&ret, 0, sizeof(i2s_capabilities_t));
        return ret;
    }

    return i2s_capabilities;
}
/**
  \brief       control I2S power.
  \param[in]   handle  i2s handle to operate.
  \param[in]   state   power state.\ref csi_power_stat_e.
  \return      error code
*/
int32_t csi_i2s_power_control(i2s_handle_t handle, csi_power_stat_e state)
{
    I2S_NULL_PARAM_CHK(handle);
    return ERR_I2S(DRV_ERROR_UNSUPPORTED);
}
/**
  \brief       config i2s protocol.
  \param[in]   handle       i2s handle to operate.
  \param[in]   protocol     i2s protocol \ref i2s_protocol_e.
  \return      0 for success, negative for error code
*/
static int32_t silan_i2s_config_protocol(i2s_handle_t handle, i2s_protocol_e protocol)
{
    I2S_NULL_PARAM_CHK(handle);

    ADEV_I2S_HANDLE *get_handle = (ADEV_I2S_HANDLE *)handle;

    switch (protocol) {
        case I2S_PROTOCOL_I2S:
            get_handle->i2s_cfg.prot = I2S_PROT_I2S;
            break;

        case I2S_PROTOCOL_MSB_JUSTIFIED:
            get_handle->i2s_cfg.prot = I2S_PROT_LEFT;
            break;

        case I2S_PROTOCOL_LSB_JUSTIFIED:
            get_handle->i2s_cfg.prot = I2S_PROT_RIGHT;
            break;

        case I2S_PROTOCOL_PCM:
            get_handle->i2s_cfg.prot = I2S_PROT_PCM;
            break;

        default:
            return ERR_I2S(I2S_ERROR_PROTOCOL);
    }

    return 0;
}

/**
  \brief       config i2s sample width.
  \param[in]   handle       i2s handle to operate.
  \param[in]   samplewidth  size of sample in bits
  \return      0 for success, negative for error code
*/
static int32_t silan_i2s_config_sample_width(i2s_handle_t handle, uint32_t samplewidth)
{
    I2S_NULL_PARAM_CHK(handle);

    if (samplewidth != 16 && samplewidth != 24) {
        return ERR_I2S(I2S_ERROR_DATA_SIZE);
    }

    ADEV_I2S_HANDLE *get_handle = (ADEV_I2S_HANDLE *)handle;
    get_handle->adev_cfg.width = samplewidth;

    return 0;
}

/**
  \brief       config i2s work mode.
  \param[in]   mode         work mode of i2s \ref Work mode of i2s.
  \return      0 for success, negative for error code
*/
static int32_t silan_i2s_config_work_mode(i2s_handle_t handle, i2s_mode_e mode)
{
    I2S_NULL_PARAM_CHK(handle);

    ADEV_I2S_HANDLE *get_handle = (ADEV_I2S_HANDLE *)handle;

    switch (mode) {
        case I2S_MODE_TX_MASTER:
            get_handle->i2s_cfg.master = CLK_MASTER;
            get_handle->i2s_cfg.tr     = I2S_TR_TO;
            break;

        case I2S_MODE_TX_SLAVE:
            get_handle->i2s_cfg.master = CLK_SLAVE;
            get_handle->i2s_cfg.tr     = I2S_TR_TO;
            break;

        case I2S_MODE_RX_MASTER:
            get_handle->i2s_cfg.master = CLK_MASTER;
            get_handle->i2s_cfg.tr     = I2S_TR_RO;
            break;

        case I2S_MODE_RX_SLAVE:
            get_handle->i2s_cfg.master = CLK_SLAVE;
            get_handle->i2s_cfg.tr     = I2S_TR_RO;
            break;

        default :
            return ERR_I2S(DRV_ERROR_UNSUPPORTED);
    }

    if (mode == I2S_MODE_TX_SLAVE || mode == I2S_MODE_RX_SLAVE) {
        get_handle->i2s_cfg.codec  = CODEC_ID_EXT;
    }

    if (mode == I2S_MODE_TX_MASTER || mode == I2S_MODE_TX_SLAVE) {
        silan_adev_i2s_set_gain((ADEV_I2S_HANDLE *)handle, -150, -150);
    } else {
        silan_adev_i2s_set_gain((ADEV_I2S_HANDLE *)handle, 0, 0);
    }

    return 0;
}

/**
  \brief       config i2s sample rate.
  \param[in]   handle       i2s handle to operate.
  \param[in]   rate         audio sample rate.
  \return      0 for success, negative for error code
*/
static int32_t silan_i2s_config_sample_rate(i2s_handle_t handle, uint32_t rate)
{
    I2S_NULL_PARAM_CHK(handle);

    switch (rate) {
        case 8000 :
        case 16000:
        case 32000:
        case 64000:
        case 12000:
        case 24000:
        case 48000:
        case 96000:
        case 11025:
        case 22050:
        case 44100:
        case 88200:
            break;

        default:
            return ERR_I2S(I2S_ERROR_AUDIO_FREQ);
    }

    ADEV_I2S_HANDLE *get_handle = (ADEV_I2S_HANDLE *)handle;
    get_handle->adev_cfg.sample_rate = rate;

    silan_adev_i2s_open((ADEV_I2S_HANDLE *)get_handle);
    silan_adev_i2s_set_rate((ADEV_I2S_HANDLE *)get_handle, rate);

    silan_adev_i2s_submit((ADEV_I2S_HANDLE *)get_handle);
    silan_adev_i2s_start((ADEV_I2S_HANDLE *)get_handle);
    return 0;
}
/**
  \brief       config i2s attributes.
  \param[in]   handle       i2s handle to operate.
  \param[in]   fmt          i2s mode \ref i2s_format_e.
  \param[in]   samplewidth  size of sample in bits
  \param[in]   mode         work mode of i2s \ref Work mode of i2s.
  \param[in]   rate         audio sample rate.
  \return      0 for success, negative for error code
*/
int32_t csi_i2s_config(i2s_handle_t handle, i2s_config_t *config)
{
    I2S_NULL_PARAM_CHK(handle);

    int32_t ret = silan_i2s_config_protocol(handle, config->cfg.protocol);

    if (ret < 0) {
        return ret;
    }

    ret = silan_i2s_config_sample_width(handle, config->cfg.width);

    if (ret < 0) {
        return ret;
    }

    ret = silan_i2s_config_work_mode(handle, config->cfg.mode);

    if (ret < 0) {
        return ret;
    }

    ret = silan_i2s_config_sample_rate(handle, config->rate);

    if (ret < 0) {
        return ret;
    }

    return 0;
}

/**
  \brief       sending data to i2s transmitter.
  \param[in]   handle       i2s handle to operate.
  \param[in]   data         Pointer to buffer for data to send
  \param[in]   data_size    size of tranmitter data
  \return      0 for success, negative for error code
*/
uint32_t csi_i2s_send(i2s_handle_t handle, const uint8_t *data, uint32_t length)
{
    I2S_NULL_PARAM_CHK(handle);
    I2S_NULL_PARAM_CHK(data);

    if (length == 0) {
        return ERR_I2S(DRV_ERROR_PARAMETER);
    }

    uint32_t lli_size = length;

    silan_adev_write(handle, lli_size);

    return 0;
}

/**
  \brief       control the i2s transfer.
  \param[in]   handle       i2s handle to operate.
  \param[in]   cmd          i2s ctrl command
  \return      0 for success, negative for error code
*/
int32_t csi_i2s_send_ctrl(i2s_handle_t handle, i2s_ctrl_e cmd)
{
    I2S_NULL_PARAM_CHK(handle);

    if (cmd == I2S_STREAM_PAUSE || cmd == I2S_STREAM_STOP) {
        silan_adev_i2s_stop((ADEV_I2S_HANDLE *)handle);
    } else if (cmd == I2S_STREAM_RESUME || cmd == I2S_STREAM_START) {
        silan_adev_i2s_revoke((ADEV_I2S_HANDLE *)handle);
    } else {
        return -1;
    }

    return 0;
}

/**
  \brief       receiving data from i2s receiver.
  \param[in]   handle       i2s handle to operate.
  \param[in]   data         Pointer to buffer for data to receive from i2s receiver
  \param[in]   data_size    size of receiver data
  \return      0 for success, negative for error code
*/
uint32_t csi_i2s_receive(i2s_handle_t handle, uint8_t *buf, uint32_t length)
{
    I2S_NULL_PARAM_CHK(handle);
    I2S_NULL_PARAM_CHK(buf);

    if (length == 0) {
        return ERR_I2S(DRV_ERROR_PARAMETER);
    }

    uint32_t lli_size = length;

    silan_adev_read(handle, lli_size);

    return 0;
}

/**
  \brief       control the i2s receive.
  \param[in]   handle       i2s handle to operate.
  \param[in]   cmd          i2s ctrl command
  \return      0 for success, negative for error code
*/
int32_t csi_i2s_receive_ctrl(i2s_handle_t handle, i2s_ctrl_e cmd)
{
    I2S_NULL_PARAM_CHK(handle);

    if (cmd == I2S_STREAM_PAUSE || cmd == I2S_STREAM_STOP) {
        silan_adev_i2s_stop((ADEV_I2S_HANDLE *)handle);
    } else if (cmd == I2S_STREAM_RESUME || cmd == I2S_STREAM_START) {
        silan_adev_i2s_revoke((ADEV_I2S_HANDLE *)handle);
    } else {
        return -1;
    }

    return 0;
}

/**
  \brief       Get i2s status.
  \param[in]   handle i2s handle to operate.
  \return      i2s status \ref i2s_status_e
*/
i2s_status_t csi_i2s_get_status(i2s_handle_t handle)
{
    i2s_status_t status = {0};

    if (handle == NULL) {
        i2s_status_t status = {0};
        return status;
    }

    return status;
}
