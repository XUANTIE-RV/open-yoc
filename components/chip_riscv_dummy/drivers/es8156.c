#include "drv/common.h"
#include "es8156.h"

struct es8156_reg_list {
    unsigned char reg_addr;
    unsigned char reg_val;
};

static const struct es8156_reg_list es8156_power_up_cfg[] = {
    { 0x20, 0x2A },
    { 0x21, 0x3C },
    { 0x22, 0x08 },
    { 0x23, 0xCA },
    { 0x0A, 0x01 },
    { 0x0B, 0x01 },
    { 0x11, 0x30 },
    { 0x14, 0xBF },
    { 0x01, 0x21 },
    { 0x02, 0x04 },
    { 0x0D, 0x14 },
    { 0x08, 0x3F },
    { 0x00, 0x02 },
    { 0x00, 0x03 },
    { 0x25, 0x20 },
};

static const struct es8156_reg_list es8156_power_down_cfg[] = {
    { 0x14, 0x00 },
    { 0x19, 0x72 },
    { 0x21, 0x1F },
    { 0x22, 0x02 },
    { 0x25, 0x21 },
    { 0x25, 0x01 },
    { 0x25, 0x87 },
    { 0x08, 0x00 },
};

static int32_t es8156_reg_write(es8156_dev_t *dev, uint8_t reg_addr, uint8_t val)
{
    int32_t ret;
    uint8_t send_data[2];
    send_data[0] = reg_addr;
    send_data[1] = val;
    ret = dev->es8156_iic_write(dev->es8156_addr, send_data, 2);

    if (ret == 2) {
        ret = 0;
    } else {
        ret = -1;
    }

    mdelay(1);
    return ret;
}

static uint32_t es8156_reg_read(es8156_dev_t *dev, uint8_t reg_addr, uint8_t *val)
{
    uint32_t ret = 0;
    ret += dev->es8156_iic_write(dev->es8156_addr, &reg_addr, 1);
    ret += dev->es8156_iic_read(dev->es8156_addr, val, 1);

    if (ret == 2) {
        ret = 0;
    } else {
        ret = -1;
    }

    mdelay(1);
    return ret;
}

static inline int32_t reg_set_bit(es8156_dev_t *dev, uint8_t reg, uint8_t ops, uint8_t mask)
{
    uint8_t val = 0;
    int ret = es8156_reg_read(dev, reg, &val);

    if (ret != 0) {
        return -1;
    }

    val |= mask << ops;

    return es8156_reg_write(dev, reg, val);
}

static inline int32_t reg_clear_bit(es8156_dev_t *dev, uint8_t reg, uint8_t ops, uint8_t mask)
{
    uint8_t val = 0;
    int ret = es8156_reg_read(dev, reg, &val);

    if (ret != 0) {
        return -1;
    }

    val &= ~(mask << ops);

    return es8156_reg_write(dev, reg, val);
}

static inline int32_t es8156_update_bits(es8156_dev_t *dev, unsigned char reg, unsigned char mask, unsigned char value)
{
    uint8_t val_old, val_new;

    es8156_reg_read(dev, reg, &val_old);
    val_new = (val_old & ~mask) | (value & mask);

    if (val_new != val_old) {
        es8156_reg_write(dev, reg, val_new);
    }

    return 0;
}


static inline int32_t es8156_i2s_mode(es8156_dev_t *dev, es8156_i2s_mode_t i2s_mode)
{
    int32_t ret = 0;

    if (i2s_mode == ES8156_I2S_MODE_MASTER) {
        ret = es8156_update_bits(dev, ES8156_MODE_CONFIG_REG02, 0x1, 1U);
    } else {
        ret = es8156_update_bits(dev, ES8156_MODE_CONFIG_REG02, 0x1, 0U);
    }

    return ret;

}

static inline int32_t es8156_i2s_protocol(es8156_dev_t *dev, es8156_protocol_t i2s_protocol)
{
    int32_t ret = 0;

    switch (i2s_protocol) {
        case ES8156_NORMAL_I2S:
            ret = es8156_update_bits(dev, ES8156_SDP_CFG1_REG11, 0x03, 0x00);
            break;

        case ES8156_NORMAL_LSB_JUSTIFIED:
            ret = es8156_update_bits(dev, ES8156_SDP_CFG1_REG11, 0x03, 0x01);
            break;

        default:
            ret = es8156_update_bits(dev, ES8156_SDP_CFG1_REG11, 0x03, 0x00);
            break;
    }

    return ret;
}

static inline int32_t es8156_i2s_data_len(es8156_dev_t *dev, es8156_data_len_t i2s_data_len)
{
    int32_t ret = 0;

    switch (i2s_data_len) {
        case ES8156_16BIT_LENGTH:
            ret = es8156_update_bits(dev, ES8156_SDP_CFG1_REG11, 0x30, 0x30);
            break;

        case ES8156_18BIT_LENGTH:
            ret = es8156_update_bits(dev, ES8156_SDP_CFG1_REG11, 0x70, 0x20);
            break;

        case ES8156_20BIT_LENGTH:
            ret = es8156_update_bits(dev, ES8156_SDP_CFG1_REG11, 0x70, 0x10);
            break;

        case ES8156_24BIT_LENGTH:
            ret = es8156_update_bits(dev, ES8156_SDP_CFG1_REG11, 0x70, 0x00);
            break;

        case ES8156_32BIT_LENGTH:
            ret = es8156_update_bits(dev, ES8156_SDP_CFG1_REG11, 0x70, 0x40);
            break;

        default:
            ret = es8156_update_bits(dev, ES8156_SDP_CFG1_REG11, 0x70, 0x60);
            break;
    }

    return ret;
}

static inline int32_t es8156_clk_divider(es8156_dev_t *dev, es8156_i2s_sclk_freq_t i2s_sclk_freq, es8156_mclk_freq_t i2s_mclk_freq)
{
    int32_t ret = 0;
    ret |= es8156_update_bits(dev, ES8156_M_CLK_CTL_REG05, 0x7f, i2s_sclk_freq);    ///< set sclk
    ret |= es8156_reg_write(dev, ES8156_M_LRCK_DIVH_REG03, (uint8_t)(i2s_mclk_freq >> 8U));    ///< set fs
    ret |= es8156_reg_write(dev, ES8156_M_LRCK_DIVL_REG04, (uint8_t)i2s_mclk_freq);
    return ret;
}

static inline int32_t es8156_set_sample_rate(es8156_dev_t *dev, es8156_i2s_sample_rate_t rate)
{
    int32_t ret = 0;

    switch (rate) {
        case ES8156_I2S_SAMPLE_RATE_48000:
            ret |= es8156_update_bits(dev, ES8156_MODE_CONFIG_REG02, 0x2U, 0U);
            ret |= es8156_update_bits(dev, ES8156_DAC_NS_REG10, 0x7fU, 64U);
            ret |= es8156_update_bits(dev, ES8156_MAIN_CLK_REG01, 0x1fU, 1U);
            ret |= es8156_update_bits(dev, ES8156_MAIN_CLK_REG01, 0xc0U, 0U);
            ret |= es8156_update_bits(dev, ES8156_MISC_CTL1_REG07, 0x70U, 1U);
            break;

        case ES8156_I2S_SAMPLE_RATE_32000:
            ret |= es8156_update_bits(dev, ES8156_MODE_CONFIG_REG02, 0x2U, 0U);
            ret |= es8156_update_bits(dev, ES8156_DAC_NS_REG10, 0x7fU, 64U);
            ret |= es8156_update_bits(dev, ES8156_MAIN_CLK_REG01, 0xffU, 0x43U);
            ret |= es8156_update_bits(dev, ES8156_MISC_CTL1_REG07, 0x70U, 1U);
            break;

        case ES8156_I2S_SAMPLE_RATE_44100:
            ret |= es8156_update_bits(dev, ES8156_MODE_CONFIG_REG02, 0x2U, 0U);
            ret |= es8156_update_bits(dev, ES8156_DAC_NS_REG10, 0x7fU, 64U);
            ret |= es8156_update_bits(dev, ES8156_MAIN_CLK_REG01, 0xffU, 1U);
            ret |= es8156_update_bits(dev, ES8156_MISC_CTL1_REG07, 0x70U, 1U);
            break;

        default:
            break;
    }

    return ret;
}


int32_t es8156_set_left_channel_mute(es8156_dev_t *dev, bool en)
{
    if (en) {
        return es8156_update_bits(dev, ES8156_MUTE_CTL_REG13, 0x04, 0x01);
    }

    return es8156_update_bits(dev, ES8156_MUTE_CTL_REG13, 0x04, 0x00);
}

int32_t es8156_set_right_channel_mute(es8156_dev_t *dev, bool en)
{
    if (en) {
        return es8156_update_bits(dev, ES8156_MUTE_CTL_REG13, 0x02, 0x01);
    }

    return es8156_update_bits(dev, ES8156_MUTE_CTL_REG13, 0x02, 0x00);
}

int32_t es8156_set_software_mute(es8156_dev_t *dev, bool en)
{
    if (en) {
        return es8156_update_bits(dev, ES8156_MUTE_CTL_REG13, 0x10, 0x00);
    }

    return es8156_update_bits(dev, ES8156_MUTE_CTL_REG13, 0x10, 0x10);
}

int32_t es8156_set_volume(es8156_dev_t *dev, unsigned char gain)
{
    return es8156_reg_write(dev, ES8156_VOL_CTL_REG14, gain);
}

int32_t es8156_init(es8156_dev_t *dev, es8156_config_t *es8156_config)
{
    int32_t ret;
    uint32_t cnt;

    for (cnt = 0; cnt < sizeof(es8156_power_up_cfg) / sizeof(es8156_power_up_cfg[0]); cnt++) {
        ret = es8156_reg_write(dev, es8156_power_up_cfg[cnt].reg_addr, es8156_power_up_cfg[cnt].reg_val);

        if (ret != 0) {
            return -1;
        }
    }

    ret |= es8156_i2s_mode(dev, es8156_config->i2s_mode);
    ret |= es8156_i2s_protocol(dev, es8156_config->i2s_protocol);
    ret |= es8156_i2s_data_len(dev, es8156_config->data_len);
    ret |= es8156_set_sample_rate(dev, es8156_config->i2s_rate);

    if (es8156_config->i2s_mode == ES8156_I2S_MODE_MASTER) {
        ret |= es8156_clk_divider(dev, es8156_config->i2s_sclk_freq, es8156_config->mclk_freq);
    }

    return ret;
}

int32_t es8156_uninit(es8156_dev_t *dev)
{
    int32_t ret;
    uint32_t cnt;

    for (cnt = 0; cnt < sizeof(es8156_power_down_cfg) / sizeof(es8156_power_down_cfg[0]); cnt++) {
        ret = es8156_reg_write(dev, es8156_power_down_cfg[cnt].reg_addr, es8156_power_down_cfg[cnt].reg_val);

        if (ret != 0) {
            return -1;
        }
    }

    return ret;
}

void es8156_read_all_register(es8156_dev_t *dev, uint8_t *buf)
{
    for (uint32_t cnt = 0; cnt < 38U; cnt ++) {
        es8156_reg_read(dev, cnt, &buf[cnt]);
    }
}

void es8156_all_data_left_channel(es8156_dev_t *dev)
{
    es8156_reg_write(dev, ES8156_MISC_CTL3_REG18, 0x10);
}

void es8156_all_data_right_channel(es8156_dev_t *dev)
{
    es8156_reg_write(dev, ES8156_MISC_CTL3_REG18, 0x20);
}