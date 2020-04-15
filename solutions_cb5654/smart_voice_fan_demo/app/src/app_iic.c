/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <pin_name.h>
#include <pinmux.h>
#include <drv/gpio.h>
#include <drv/iic.h>
#include "board_config.h"
#include "shell.h"
#include "app_printf.h"

#define  CK_IIC_SLAVE_ADDR         0x50
#define  EEPROM_PAGE_SIZE          0x20
#define  EEPROM_SIZE               8192
#define  EEPROM_SUB_ADDR_START     0x0000
#define  EEPROM_SUB_ADDR_END       0x1FFF

#define PIN_IIC_SDA_FUNC  3
#define PIN_IIC_SCL_FUNC  3


static iic_handle_t pcsi_iic;
static volatile uint8_t cb_transfer_flag = 0;

extern void mdelay(uint32_t ms);

static void iic_event_cb_fun(int32_t idx, iic_event_e event)
{
    if (event == IIC_EVENT_TRANSFER_DONE) {
        cb_transfer_flag = 1;
    }
}

void example_pin_iic_init(void)
{
	drv_pinmux_config(I2C_SCL, PIN_IIC_SCL_FUNC);
	drv_pinmux_config(I2C_SDA, PIN_IIC_SDA_FUNC);
}

static void cmd_app_func(int argc, char *argv[])
{
    bool xfer_pending = false;
    uint8_t write_data[EEPROM_PAGE_SIZE + 2] = {0x0, EEPROM_PAGE_SIZE, 0};
    uint8_t read_data[EEPROM_PAGE_SIZE + 2] = {0x0, EEPROM_PAGE_SIZE, 0};
    uint8_t i = 0;
    int32_t ret;

    cb_transfer_flag = 0;

    for (i = 2; i < sizeof(write_data); i++) {
        write_data[i] = i - 2;
    }

    example_pin_iic_init();

    pcsi_iic = csi_iic_initialize(1, iic_event_cb_fun);

    if (pcsi_iic == NULL) {
        LOGE("csi_iic_initialize error\n");
        return;
    }

    ret = csi_iic_config(pcsi_iic, IIC_MODE_MASTER, IIC_BUS_SPEED_STANDARD, IIC_ADDRESS_7BIT, CK_IIC_SLAVE_ADDR);

    if (ret < 0) {
        LOGE("csi_iic_config error\n");
        return;
    }

    ret = csi_iic_master_send(pcsi_iic, CK_IIC_SLAVE_ADDR, write_data, sizeof(write_data), xfer_pending);

    if (ret < 0) {
        LOGE("csi_iic_master_send error\n");
        return;
    }

    while (!cb_transfer_flag);

    mdelay(5);

    cb_transfer_flag = 0;
    ret = csi_iic_master_send(pcsi_iic, CK_IIC_SLAVE_ADDR, read_data, 2, xfer_pending);

    if (ret < 0) {
        LOGE("csi_iic_master_send error\n");
        return;
    }

    while (!cb_transfer_flag);

    cb_transfer_flag = 0;
    ret = csi_iic_master_receive(pcsi_iic, CK_IIC_SLAVE_ADDR, read_data + 2, sizeof(read_data) - 2, xfer_pending);

    if (ret < 0) {
        LOGE("csi_iic_master_receive error\n");
        return;
    }

    while (!cb_transfer_flag);


    LOGD("write_data,read_data:\n");

    for (i = 2; i < EEPROM_PAGE_SIZE + 2; i++) {
        LOGD("%x,%x \t", write_data[i], read_data[i]);

        if (((i + 3) % 4) == 0) {
            LOGD("\n");
        }

        if (write_data[i] != read_data[i]) {
            LOGE("\ntest iic write and read failed\n");
            return;
        }
    }

    LOGI("\ntest write and read passed\n");

    /* test abort fun */
    memset(read_data + 2, 0x0, EEPROM_PAGE_SIZE);
    cb_transfer_flag = 0;
    ret = csi_iic_master_send(pcsi_iic, CK_IIC_SLAVE_ADDR, read_data, 2, xfer_pending);

    if (ret < 0) {
        LOGE("csi_iic_master_send error\n");
        return;
    }

    while (!cb_transfer_flag);

    cb_transfer_flag = 0;
    ret = csi_iic_master_receive(pcsi_iic, CK_IIC_SLAVE_ADDR, read_data + 2, sizeof(read_data) - 2, xfer_pending);

    if (ret < 0) {
        LOGE("csi_iic_master_receive error\n");
        return;
    }

    ret = csi_iic_abort_transfer(pcsi_iic);

    if (ret < 0) {
        LOGE("csi_iic_abort_transfer error\n");
        return;
    }

    LOGD("write_data,read_data:\n");

    for (i = 2; i < EEPROM_PAGE_SIZE + 2; i++) {
        LOGD("%x,%x\t", write_data[i], read_data[i]);

        if (write_data[i] != read_data[i]) {
            break;
        }
    }

    if (i == EEPROM_PAGE_SIZE) {
        LOGE("\ntest abort function failed\n");
        return;
    }

    LOGI("\ntest abort function passed\n");

    ret = csi_iic_uninitialize(pcsi_iic);

    if (ret < 0) {
        LOGE("csi_iic_uninitialize error\n");
        return;
    }

    LOGI("test iic successfully\n");
    return;
}


void shell_reg_cmd_iic(void)
{
    shell_cmd_t cmd_info = {"iic", "iic test", cmd_app_func};

    shell_register_command(&cmd_info);
}
