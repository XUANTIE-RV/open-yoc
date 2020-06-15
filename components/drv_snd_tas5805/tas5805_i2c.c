/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include <iic.h>

#include "tas5805.h"
#include "tas5805_init_table.h"

#define TAG "i2c"

#define  TAS5805_SLAVE_ADDR         0x2c

static aos_dev_t *iic_init(int id)
{
    aos_dev_t *i2c_dev = iic_open_id("iic", id);
    iic_config_t config = {
        MODE_MASTER,
        BUS_SPEED_STANDARD,
        ADDR_7BIT,
        TAS5805_SLAVE_ADDR
    };
    iic_config(i2c_dev, &config);

    return i2c_dev;
}

static void iic_deinit(aos_dev_t *dev)
{
    iic_close(dev);
}

static int tas5805_write_byte(aos_dev_t *i2c_dev, uint8_t reg_addr, uint8_t reg_data)
{
    uint8_t data[2] = {0};

    data[0] = reg_addr;
    data[1] = reg_data;

    return (iic_master_send(i2c_dev, TAS5805_SLAVE_ADDR, data, 2, AOS_WAIT_FOREVER));
}

static void tas5805_read_byte(aos_dev_t *i2c_dev, uint8_t reg_addr, uint8_t *reg_data)
{
    iic_master_send(i2c_dev, TAS5805_SLAVE_ADDR, &reg_addr, 1, AOS_WAIT_FOREVER);
    iic_master_recv(i2c_dev, TAS5805_SLAVE_ADDR, reg_data, 1, AOS_WAIT_FOREVER);
}

static inline int get_volume_index(int vol)
{
    int index;

    index = vol;

    if (index < TAS5805M_VOLUME_MIN) {
        index = TAS5805M_VOLUME_MIN;
    }

    if (index > TAS5805M_VOLUME_MAX) {
        index = TAS5805M_VOLUME_MAX;
    }

    return index;
}

void vol_set(aos_dev_t *i2c_dev, int vol)
{
    uint32_t volume_hex = tas5805m_volume[vol];;
    uint8_t byte4;
    uint8_t byte3;
    uint8_t byte2;
    uint8_t byte1;

    byte4 = ((volume_hex >> 24) & 0xFF);
    byte3 = ((volume_hex >> 16) & 0xFF);
    byte2 = ((volume_hex >> 8)  & 0xFF);
    byte1 = ((volume_hex >> 0)  & 0xFF);

    //w 58 00 00
    tas5805_write_byte(i2c_dev, TAS5805M_REG_00, TAS5805M_PAGE_00);
    //w 58 7f 8c
    tas5805_write_byte(i2c_dev, TAS5805M_REG_7F, TAS5805M_BOOK_8C);
    //w 58 00 2a
    tas5805_write_byte(i2c_dev, TAS5805M_REG_00, TAS5805M_PAGE_2A);
    //w 58 24 xx xx xx xx
    tas5805_write_byte(i2c_dev, TAS5805M_REG_24, byte4);
    tas5805_write_byte(i2c_dev, TAS5805M_REG_25, byte3);
    tas5805_write_byte(i2c_dev, TAS5805M_REG_26, byte2);
    tas5805_write_byte(i2c_dev, TAS5805M_REG_27, byte1);

    //w 58 28 xx xx xx xx
    tas5805_write_byte(i2c_dev, TAS5805M_REG_28, byte4);
    tas5805_write_byte(i2c_dev, TAS5805M_REG_29, byte3);
    tas5805_write_byte(i2c_dev, TAS5805M_REG_2A, byte2);
    tas5805_write_byte(i2c_dev, TAS5805M_REG_2B, byte1);

    LOGE(TAG, "vol set ok");
}

aos_dev_t *tas5805_i2c_init(int id)
{
    /* init i2c */
    uint8_t val;
    int ret;
    aos_dev_t *i2c_dev = iic_init(id);

    if (i2c_dev == NULL) {
        LOGE(TAG, "csi_iic_initialize error\n");
        return NULL;
    }

    LOGE(TAG, "iic send start(%d) \n", sizeof(tas5805m_init_sequence) / sizeof(struct reg_sequence));

    for (int i = 0 ; i < sizeof(tas5805m_init_sequence) / sizeof(struct reg_sequence); i++) {
        ret = tas5805_write_byte(i2c_dev, tas5805m_init_sequence[i].reg, tas5805m_init_sequence[i].val);
        if (ret < 0) {
            LOGE(TAG, "iic send err (%d)", i);
            iic_deinit(i2c_dev);
            return NULL;
        }
    }
    tas5805_read_byte(i2c_dev, 0x02, &val);

    vol_set(i2c_dev, 50);

    LOGE(TAG, "codec config end! start init and config i2s(%d)\n", val);

    return i2c_dev;
}