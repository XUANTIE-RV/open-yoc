/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <sys_clk.h>
#include <drv/iic.h>
#include <drv/pin.h>
#include "es_port.h"

static csi_iic_t iic_codec;

csi_error_t es_port_iic_init(void)
{
    csi_error_t ret;

    /* iic pinmux init */
    csi_pin_set_mux(CODEC_IIC_SDA, CODEC_IIC_SDA_FUNC);
    csi_pin_set_mux(CODEC_IIC_SCL, CODEC_IIC_SCL_FUNC);

    ret = csi_iic_init(&iic_codec, CODEC_IIC_IDX);

    if (ret != CSI_OK) {
        printf("iic init error\n");
        return CSI_ERROR;
    }

    ret = csi_iic_mode(&iic_codec, IIC_MODE_MASTER);

    if (ret != CSI_OK) {
        printf("iic_set_mode error\n");
        return CSI_ERROR;
    }

    ret = csi_iic_addr_mode(&iic_codec, IIC_ADDRESS_7BIT);

    if (ret != CSI_OK) {
        printf("iic_set_addr_mode error\n");
        return CSI_ERROR;
    }

    ret = csi_iic_speed(&iic_codec, IIC_BUS_SPEED_STANDARD);

    if (ret != CSI_OK) {
        printf("iic_set_speed error\n");
        return CSI_ERROR;
    }

    return ret;
}

csi_error_t es_port_iic_read(uint8_t addr, uint8_t *data, uint32_t date_len)
{
    return csi_iic_master_receive(&iic_codec, addr, data, date_len, 100000U);
}

csi_error_t es_port_iic_write(uint8_t addr, uint8_t *data, uint32_t date_len)
{
    return csi_iic_master_send(&iic_codec, addr, data, date_len, 100000U);
}

