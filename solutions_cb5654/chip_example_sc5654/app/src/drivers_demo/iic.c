/*
 * Copyright (C) 2017 C-SKY Microsystems Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <app_config.h>

#include <stdio.h>

#include <drv/iic.h>
#include <soc.h>
#include <pin_name.h>
#include <pinmux.h>

#include <aos/cli.h>
#include <aos/aos.h>


#define  IIC_SLAVE_ADDR         0x50  /* different addr for each I2C dev */
#define  PAGE_SIZE          0x20

static iic_handle_t iic_handle;
static volatile uint8_t cb_transfer_flag = 0;

typedef struct {
    int32_t    scl;
    int32_t    sda;
	int32_t    scl_func;
	int32_t    sda_func;
    uint16_t cfg_idx;
} iic_pinmap_t;
const static iic_pinmap_t g_iic_pinmap[] = {
    {
        PA6,
        PA7,
        PA6_I2C1_SCL,
        PA7_I2C1_SDA,
        1
    },
    {
        PC12,
        PC13,
        PC12_I2C2_SCL,
        PC13_I2C2_SDA,
        2
    },
};



static void iic_event_cb_fun(int32_t idx, iic_event_e event)
{
    if (event == IIC_EVENT_TRANSFER_DONE) {
        cb_transfer_flag = 1;
    }
}

void map_pin_iic_init(int32_t iic_idx)
{
    for(int i = 0; i < sizeof(g_iic_pinmap) / sizeof(iic_pinmap_t); i++) {
        if(g_iic_pinmap[i].cfg_idx == iic_idx) {
            drv_pinmux_config(g_iic_pinmap[i].scl, g_iic_pinmap[i].scl_func);
            drv_pinmux_config(g_iic_pinmap[i].sda, g_iic_pinmap[i].sda_func);
            break;
        }
    }
}

static int test_iic_handle(int32_t iic_id)
{
    uint8_t write_data[PAGE_SIZE + 2] = {0x0, PAGE_SIZE, 0};
    uint8_t read_data[PAGE_SIZE + 2] = {0x0, PAGE_SIZE, 0};
    uint8_t i = 0;
    int32_t ret;

    cb_transfer_flag = 0;

    for (i = 2; i < sizeof(write_data); i++) {
        write_data[i] = i - 2;
    }

    map_pin_iic_init(iic_id);

    iic_handle = csi_iic_initialize((iic_id - 1), iic_event_cb_fun);

    if (iic_handle == NULL) {
        printf("csi_iic_initialize error\n");
        return -1;
    }

    ret = csi_iic_config(iic_handle, IIC_MODE_MASTER, IIC_BUS_SPEED_STANDARD, IIC_ADDRESS_7BIT, IIC_SLAVE_ADDR);

    if (ret < 0) {
        printf("csi_iic_config error\n");
        return -1;
    }

    ret = csi_iic_master_send(iic_handle, IIC_SLAVE_ADDR, write_data, sizeof(write_data), false);

    if (ret < 0) {
        printf("csi_iic_master_send error\n");
        return -1;
    }

    while (!cb_transfer_flag);

    aos_msleep(5);

    cb_transfer_flag = 0;
    ret = csi_iic_master_send(iic_handle, IIC_SLAVE_ADDR, read_data, 2, false);

    if (ret < 0) {
        printf("csi_iic_master_send error\n");
        return -1;
    }

    while (!cb_transfer_flag);

    cb_transfer_flag = 0;
    ret = csi_iic_master_receive(iic_handle, IIC_SLAVE_ADDR, read_data + 2, sizeof(read_data) - 2, false);

    if (ret < 0) {
        printf("csi_iic_master_receive error\n");
        return -1;
    }

    while (!cb_transfer_flag);


    printf("write_data,read_data:\n");

    for (i = 2; i < PAGE_SIZE + 2; i++) {
        printf("%x,%x \t", write_data[i], read_data[i]);

        if (((i + 3) % 4) == 0) {
            printf("\n");
        }

        if (write_data[i] != read_data[i]) {
            printf("\ntest at24c64 write and read failed\n");
            return -1;
        }
    }

    printf("\ntest at24c64 write and read passed\n");

    /* test abort fun */
    memset(read_data + 2, 0x0, PAGE_SIZE);
    cb_transfer_flag = 0;
    ret = csi_iic_master_send(iic_handle, IIC_SLAVE_ADDR, read_data, 2, false);

    if (ret < 0) {
        printf("csi_iic_master_send error\n");
        return -1;
    }

    while (!cb_transfer_flag);

    cb_transfer_flag = 0;
    ret = csi_iic_master_receive(iic_handle, IIC_SLAVE_ADDR, read_data + 2, sizeof(read_data) - 2, false);

    if (ret < 0) {
        printf("csi_iic_master_receive error\n");
        return -1;
    }

    ret = csi_iic_abort_transfer(iic_handle);

    if (ret < 0) {
        printf("csi_iic_abort_transfer error\n");
        return -1;
    }

    printf("write_data,read_data:\n");

    for (i = 2; i < PAGE_SIZE + 2; i++) {
        printf("%x,%x\t", write_data[i], read_data[i]);

        if (write_data[i] != read_data[i]) {
            break;
        }
    }

    if (i == PAGE_SIZE) {
        printf("\ntest abort function failed\n");
        return -1;
    }

    printf("\ntest abort function passed\n");

    // ret = csi_iic_uninitialize(iic_handle);

    if (ret < 0) {
        printf("csi_iic_uninitialize error\n");
        return -1;
    }

    printf("test_iic_eeprom successfully\n");
    return 0;
}


static void cmd_iic_func(char *wbuf, int wbuf_len, int argc, char **argv)
{
    if(argc < 2)
        return;
    test_iic_handle(atoi(argv[1]));
}

void test_iic_cmd(void)
{
    static const struct cli_command cmd_info = {"iic", "iic command", cmd_iic_func};

    aos_cli_register_command(&cmd_info);
}

