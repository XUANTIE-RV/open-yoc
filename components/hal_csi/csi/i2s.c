#include <aos/kernel.h>
#include <stdio.h>
#include "aos/hal/i2s.h"
#include "pin_name.h"
#include "pinmux.h"
#include "string.h"
#include "csi_i2s.h"


#define I2S_BUF_SIZE 4096
#define I2S_CALLBACK_PERIOD 960

static aos_sem_t sem[6];

static void i2s_cb(int32_t idx, i2s_event_e event)
{
    if ((event == I2S_EVENT_SEND_COMPLETE) || (event == I2S_EVENT_RECEIVE_COMPLETE)) {
        aos_sem_signal(&sem[idx]);
    } else {
        printf("i2s err event %d\n", event);
    }
}

int32_t hal_i2s_init(i2s_dev_t *i2s)
{
    int32_t ret = 0;

    if (i2s == NULL) {
        return -1;
    }

    if (aos_sem_new(&sem[i2s->port], 0) != 0) {
        return -1;
    }

    i2s_handle_t handle = csi_i2s_initialize(i2s->port, (i2s_event_cb_t)i2s_cb, NULL);

    if (handle == NULL) {
        return -1;
    }

    i2s->priv = (void *)handle;
    csi_i2s_config_t i2s_config;
    memset(&i2s_config, 0, sizeof(i2s_config));

    i2s_config.cfg.left_polarity = I2S_LEFT_POLARITY_LOW;
    i2s_config.cfg.mclk_freq = I2S_MCLK_256FS;
    i2s_config.cfg.tx_mono_enable = 0;

    i2s_config.rate = i2s->config.freq;
    i2s_config.tx_buf = (uint8_t *)aos_malloc(I2S_BUF_SIZE);
    i2s_config.tx_buf_length = I2S_BUF_SIZE;
    i2s_config.tx_period = I2S_CALLBACK_PERIOD;

    switch (i2s->config.mode) {
        case MODE_MASTER_TX:
            i2s_config.cfg.mode = I2S_MODE_TX_MASTER;
            break;

        case MODE_MASTER_RX:
            i2s_config.cfg.mode = I2S_MODE_RX_MASTER;
            break;

        case MODE_SLAVE_TX:
            i2s_config.cfg.mode = I2S_MODE_TX_SLAVE;
            break;

        case MODE_SLAVE_RX:
            i2s_config.cfg.mode = I2S_MODE_RX_SLAVE;
            break;

        default:
            break;
    }

    switch (i2s->config.data_format) {
        case DATAFORMAT_16B:
            i2s_config.cfg.width = I2S_SAMPLE_16BIT;
            i2s_config.cfg.sclk_freq = I2S_SCLK_32FS;
            break;

        case DATAFORMAT_16B_EXTENDED:
            i2s_config.cfg.width = I2S_SAMPLE_16BIT;
            i2s_config.cfg.sclk_freq = I2S_SCLK_32FS;
            break;

        case DATAFORMAT_24B:
            i2s_config.cfg.width = I2S_SAMPLE_24BIT;
            i2s_config.cfg.sclk_freq = I2S_SCLK_64FS;
            break;

        case DATAFORMAT_32B:
            i2s_config.cfg.width = I2S_SAMPLE_32BIT;
            i2s_config.cfg.sclk_freq = I2S_SCLK_64FS;
            break;

        default:
            break;
    }

    switch (i2s->config.standard) {
        case STANDARD_PHILIPS:
            i2s_config.cfg.protocol = I2S_PROTOCOL_I2S;
            break;

        case STANDARD_MSB:
            i2s_config.cfg.protocol = I2S_PROTOCOL_MSB_JUSTIFIED;
            break;

        case STANDARD_LSB:
            i2s_config.cfg.protocol = I2S_PROTOCOL_LSB_JUSTIFIED;
            break;

        case STANDARD_PCM_SHORT:
            i2s_config.cfg.protocol = I2S_PROTOCOL_PCM;
            break;

        case STANDARD_PCM_LONG:
            i2s_config.cfg.protocol = I2S_PROTOCOL_PCM;
            break;

        default:
            break;
    }

    ret = csi_hal_i2s_config(handle, &i2s_config);

    if (ret != 0) {
        return -1;
    }

    csi_i2s_enable(handle, 1);

    if ((i2s->config.mode == MODE_MASTER_TX) || (i2s->config.mode == MODE_SLAVE_TX)) {
        ret = csi_i2s_send_ctrl(i2s->priv, I2S_STREAM_START);
    }

    if ((i2s->config.mode == MODE_MASTER_RX) || (i2s->config.mode == MODE_SLAVE_RX)) {
        ret = csi_i2s_receive_ctrl(i2s->priv, I2S_STREAM_START);
    }

    return ret;
}
int32_t hal_i2s_send(i2s_dev_t *i2s, const void *data, uint32_t size, uint32_t timeout)
{
    int32_t ret = 0;
    csi_i2s_send(i2s->priv, (uint8_t *)data, size);
    ret = aos_sem_wait(&sem[i2s->port], timeout);
    return ret;

}
int32_t hal_i2s_recv(i2s_dev_t *i2s, void *data, uint32_t size, uint32_t timeout)
{
    int32_t ret = 0;
    csi_i2s_receive(i2s->priv, (uint8_t *)data, size);
    ret = aos_sem_wait(&sem[i2s->port], timeout);
    return ret;
}
int32_t hal_i2s_pause(i2s_dev_t *i2s)
{
    int32_t ret = 0;

    if ((i2s->config.mode == MODE_MASTER_TX) || (i2s->config.mode == MODE_SLAVE_TX)) {
        ret = csi_i2s_send_ctrl(i2s->priv, I2S_STREAM_PAUSE);
    }

    if ((i2s->config.mode == MODE_MASTER_RX) || (i2s->config.mode == MODE_SLAVE_RX)) {
        ret = csi_i2s_receive_ctrl(i2s->priv, I2S_STREAM_PAUSE);
    }

    return ret;
}

int32_t hal_i2s_resume(i2s_dev_t *i2s)
{
    int32_t ret = 0;

    if ((i2s->config.mode == MODE_MASTER_TX) || (i2s->config.mode == MODE_SLAVE_TX)) {
        ret = csi_i2s_send_ctrl(i2s->priv, I2S_STREAM_RESUME);
    }

    if ((i2s->config.mode == MODE_MASTER_RX) || (i2s->config.mode == MODE_SLAVE_RX)) {
        ret = csi_i2s_receive_ctrl(i2s->priv, I2S_STREAM_RESUME);
    }

    return ret;
}

int32_t hal_i2s_stop(i2s_dev_t *i2s)
{
    int32_t ret = 0;

    if ((i2s->config.mode == MODE_MASTER_TX) || (i2s->config.mode == MODE_SLAVE_TX)) {
        ret = csi_i2s_send_ctrl(i2s->priv, I2S_STREAM_STOP);
    }

    if ((i2s->config.mode == MODE_MASTER_RX) || (i2s->config.mode == MODE_SLAVE_RX)) {
        ret = csi_i2s_receive_ctrl(i2s->priv, I2S_STREAM_STOP);
    }

    return ret;
}

int32_t hal_i2s_finalize(i2s_dev_t *i2s)
{
    ck_i2s_priv_v2_t *i2s_handle = (ck_i2s_priv_v2_t *)i2s->priv;
    aos_sem_free(&sem[i2s->port]);

    if ((i2s->config.mode == MODE_MASTER_TX) || (i2s->config.mode == MODE_SLAVE_TX)) {
        aos_free(i2s_handle->tx_ring_buf.fifo.buffer);
    }

    if ((i2s->config.mode == MODE_MASTER_RX) || (i2s->config.mode == MODE_SLAVE_RX)) {
        aos_free(i2s_handle->rx_ring_buf.fifo.buffer);
    }

    return csi_i2s_uninitialize((i2s_handle_t)i2s->priv);
}