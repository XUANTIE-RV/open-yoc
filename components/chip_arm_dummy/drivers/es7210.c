#include "drv/common.h"
#include "es7210.h"

struct es7210_reg_list {
    unsigned char reg_addr;
    unsigned char reg_val;
};
static const struct es7210_reg_list es7210_power_up[] = {
    { 0x00, 0x32 },
    { 0x0D, 0x09 },
    { 0x09, 0x30 },
    { 0x0A, 0x30 },
    { 0x23, 0x2a },
    { 0x22, 0x0a },
    { 0x21, 0x2a },
    { 0x20, 0x0a },
    { 0x08, 0x14 },
    { 0x11, 0x60 },
    { 0x12, 0x00 },
    { 0x40, 0xC3 },
    { 0x41, 0x70 },
    { 0x42, 0x70 },
    { 0x43, 0x1A },
    { 0x44, 0x1A },
    { 0x45, 0x1A },
    { 0x46, 0x1A },
    { 0x47, 0x08 },
    { 0x48, 0x08 },
    { 0x49, 0x08 },
    { 0x4A, 0x08 },
    { 0x07, 0x20 },
    { 0x02, 0xC1 },
    { 0x06, 0x00 },
    { 0x4B, 0x0F },
    { 0x4C, 0x0F },
    { 0x00, 0x71 },
    { 0x00, 0x41 },
};

static const struct es7210_reg_list es7210_power_down[] = {
    { 0x06, 0x00 },
    { 0x4b, 0xff },
    { 0x4c, 0xff },
    { 0x0b, 0xd0 },
    { 0x40, 0x80 },
    { 0x01, 0x7f },
    { 0x06, 0x07 },
};

static int32_t es7210_reg_write(es7210_dev_t *dev, uint8_t reg_addr, uint8_t val)
{
    int32_t ret;
    uint8_t send_data[2];
    send_data[0] = reg_addr;
    send_data[1] = val;
    ret = dev->es7210_iic_write(dev->es7210_addr, send_data, 2);

    if (ret == 2) {
        ret = 0;
    } else {
        ret = -1;
    }

    mdelay(1);
    return ret;
}

static uint32_t es7210_reg_read(es7210_dev_t *dev, uint8_t reg_addr, uint8_t *val)
{
    uint32_t ret = 0;
    ret += dev->es7210_iic_write(dev->es7210_addr, &reg_addr, 1);
    ret += dev->es7210_iic_read(dev->es7210_addr, val, 1);

    if (ret == 2) {
        ret = 0;
    } else {
        ret = -1;
    }

    mdelay(1);
    return ret;
}

static inline int32_t reg_set_bit(es7210_dev_t *dev, uint8_t reg, uint8_t ops, uint8_t mask)
{
    uint8_t val = 0;
    int ret = es7210_reg_read(dev, reg, &val);

    if (ret != 0) {
        return -1;
    }

    val |= mask << ops;

    return es7210_reg_write(dev, reg, val);
}

static inline int32_t reg_clear_bit(es7210_dev_t *dev, uint8_t reg, uint8_t ops, uint8_t mask)
{
    uint8_t val = 0;
    int ret = es7210_reg_read(dev, reg, &val);

    if (ret != 0) {
        return -1;
    }

    val &= ~(mask << ops);

    return es7210_reg_write(dev, reg, val);
}

static inline int32_t es7210_update_bits(es7210_dev_t *dev, unsigned char reg, unsigned char mask, unsigned char value)
{
    uint8_t val_old, val_new;

    es7210_reg_read(dev, reg, &val_old);
    val_new = (val_old & ~mask) | (value & mask);

    if (val_new != val_old) {
        es7210_reg_write(dev, reg, val_new);
    }

    return 0;
}

static inline int32_t es7210_reset(es7210_dev_t *dev)
{
    int32_t ret = 0;
    ret = es7210_reg_write(dev, ES7210_RESET_CTL_REG00, 0xFF);
    mdelay(10);
    return ret;

}

static inline int32_t es7210_i2s_mode(es7210_dev_t *dev, es7210_i2s_mode_t i2s_mode)
{
    int32_t ret = 0;

    if (i2s_mode == ES7210_I2S_MODE_MASTER) {
        ret = es7210_update_bits(dev, ES7210_MODE_CFG_REG08, 0x1, 0x1);
    } else {
        ret = es7210_update_bits(dev, ES7210_MODE_CFG_REG08, 0x1, 0x0);
    }

    return ret;

}

static inline int32_t es7210_i2s_protocol(es7210_dev_t *dev, es7210_protocol_t i2s_protocol)
{
    int32_t ret = 0;

    switch (i2s_protocol) {
        case ES7210_NORMAL_I2S:
            ret |= es7210_update_bits(dev, ES7210_SDP_CFG1_REG11, 0x03, 0x00);
            break;

        case ES7210_NORMAL_LSB_JUSTIFIED:
            ret |= es7210_update_bits(dev, ES7210_SDP_CFG1_REG11, 0x03, 0x01);
            break;

        default:
            ret |= es7210_update_bits(dev, ES7210_SDP_CFG1_REG11, 0x03, 0x00);
            break;
    }

    return ret;
}

static inline int32_t es7210_i2s_data_len(es7210_dev_t *dev, es7210_data_len_t i2s_data_len)
{
    int32_t ret = 0;

    switch (i2s_data_len) {
        case ES7210_16BIT_LENGTH:
            ret = es7210_update_bits(dev, ES7210_SDP_CFG1_REG11, 0xe0, 0x60);
            break;

        case ES7210_18BIT_LENGTH:
            ret = es7210_update_bits(dev, ES7210_SDP_CFG1_REG11, 0xe0, 0x40);
            break;

        case ES7210_20BIT_LENGTH:
            ret = es7210_update_bits(dev, ES7210_SDP_CFG1_REG11, 0xe0, 0x20);
            break;

        case ES7210_24BIT_LENGTH:
            ret = es7210_update_bits(dev, ES7210_SDP_CFG1_REG11, 0xe0, 0x00);
            break;

        case ES7210_32BIT_LENGTH:
            ret = es7210_update_bits(dev, ES7210_SDP_CFG1_REG11, 0xe0, 0x80);
            break;

        default:
            ret = es7210_update_bits(dev, ES7210_SDP_CFG1_REG11, 0xe0, 0x60);
            break;
    }

    return ret;
}

static inline int32_t es7210_clk_divider(es7210_dev_t *dev, es7210_i2s_sclk_freq_t i2s_sclk_freq, es7210_mclk_freq_t i2s_mclk_freq)
{
    int32_t ret = 0;
    int32_t div = 0;

    div = i2s_mclk_freq / i2s_sclk_freq;
    ret = es7210_reg_write(dev, ES7210_MST_CLK_CTL_REG03, 0x00U);    ///< sclk div=0
    ret = es7210_reg_write(dev, ES7210_MST_LRCDIVH_REG04, (uint8_t)(div >> 8U));
    ret = es7210_reg_write(dev, ES7210_MST_LRCDIVL_REG05, (uint8_t)div);

    return ret;
}

int32_t es7210_mic3_set_mute(es7210_dev_t *dev, bool en)
{
    if (en) {
        return es7210_update_bits(dev, ES7210_ADC34_MUTE_REG14, 0x01, 0x01);
    }

    return es7210_update_bits(dev, ES7210_ADC34_MUTE_REG14, 0x01, 0x00);
}

int32_t es7210_mic4_set_mute(es7210_dev_t *dev, bool en)
{
    if (en) {
        return es7210_update_bits(dev, ES7210_ADC34_MUTE_REG14, 0x02, 0x01);
    }

    return es7210_update_bits(dev, ES7210_ADC34_MUTE_REG14, 0x02, 0x00);
}

int32_t es7210_mic1_set_mute(es7210_dev_t *dev, bool en)
{
    if (en) {
        return es7210_update_bits(dev, ES7210_ADC12_MUTE_REG15, 0x01, 0x01);
    }

    return es7210_update_bits(dev, ES7210_ADC12_MUTE_REG15, 0x01, 0x00);
}

int32_t es7210_mic2_set_mute(es7210_dev_t *dev, bool en)
{
    if (en) {
        return es7210_update_bits(dev, ES7210_ADC12_MUTE_REG15, 0x02, 0x01);
    }

    return es7210_update_bits(dev, ES7210_ADC12_MUTE_REG15, 0x02, 0x00);
}

int32_t es7210_adc1_set_gain(es7210_dev_t *dev, unsigned char gain)
{
    int32_t ret = 0;

    ret = es7210_reg_write(dev, ES7210_ADC1_MAX_GAIN_REG1E, gain);

    return ret;
}

int32_t es7210_adc2_set_gain(es7210_dev_t *dev, unsigned char gain)
{
    int32_t ret = 0;

    ret = es7210_reg_write(dev, ES7210_ADC2_MAX_GAIN_REG1D, gain);

    return ret;
}

int32_t es7210_adc3_set_gain(es7210_dev_t *dev, unsigned char gain)
{
    int32_t ret = 0;

    ret = es7210_reg_write(dev, ES7210_ADC3_MAX_GAIN_REG1C, gain);

    return ret;
}

int32_t es7210_adc4_set_gain(es7210_dev_t *dev, unsigned char gain)
{
    int32_t ret = 0;

    ret = es7210_reg_write(dev, ES7210_ADC4_MAX_GAIN_REG1B, gain);

    return ret;
}

int32_t es7210_mic1_set_gain(es7210_dev_t *dev, unsigned char gain)
{
    if (gain > 14) {
        gain = 14;
    }

    es7210_update_bits(dev, ES7210_MIC1_GAIN_REG43, 0x0F, gain);
    return 0;
}

int32_t es7210_mic2_set_gain(es7210_dev_t *dev, unsigned char gain)
{
    if (gain > 14) {
        gain = 14;
    }

    es7210_update_bits(dev, ES7210_MIC2_GAIN_REG44, 0x0F, gain);
    return 0;
}

int32_t es7210_mic3_set_gain(es7210_dev_t *dev, unsigned char gain)
{
    if (gain > 14) {
        gain = 14;
    }

    es7210_update_bits(dev, ES7210_MIC3_GAIN_REG45, 0x0F, gain);
    return 0;
}

int32_t es7210_mic4_set_gain(es7210_dev_t *dev, unsigned char gain)
{
    if (gain > 14) {
        gain = 14;
    }

    es7210_update_bits(dev, ES7210_MIC4_GAIN_REG46, 0x0F, gain);
    return 0;
}

int32_t es7210_init(es7210_dev_t *dev, es7210_config_t *es7210_config)
{
    int32_t ret;
    uint32_t cnt;
    es7210_reset(dev);

    for (cnt = 0; cnt < sizeof(es7210_power_up) / sizeof(es7210_power_up[0]); cnt++) {
        ret = es7210_reg_write(dev, es7210_power_up[cnt].reg_addr, es7210_power_up[cnt].reg_val);

        if (ret != 0) {
            return -1;
        }
    }

    es7210_i2s_mode(dev, es7210_config->i2s_mode);
    es7210_i2s_protocol(dev, es7210_config->i2s_protocol);

    es7210_i2s_data_len(dev, es7210_config->data_len);

    if (es7210_config->i2s_mode == ES7210_I2S_MODE_MASTER) {
        es7210_clk_divider(dev, es7210_config->i2s_sclk_freq, es7210_config->mclk_freq);
    }

    return ret;
}

int32_t es7210_uninit(es7210_dev_t *dev)
{
    int32_t ret;
    uint32_t cnt;

    for (cnt = 0; cnt < sizeof(es7210_power_down) / sizeof(es7210_power_down[0]); cnt++) {
        ret = es7210_reg_write(dev, es7210_power_down[cnt].reg_addr, es7210_power_down[cnt].reg_val);

        if (ret != 0) {
            return -1;
        }
    }

    return ret;
}

void es7210_read_all_register(es7210_dev_t *dev, uint8_t *buf)
{
    for (uint32_t cnt = 0; cnt < 77U; cnt ++) {
        es7210_reg_read(dev, cnt, &buf[cnt]);
    }
}
