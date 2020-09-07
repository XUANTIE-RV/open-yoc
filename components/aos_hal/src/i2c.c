/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

#include <aos/kernel.h>
#include <stdio.h>
#include "drv/iic.h"
#include "pin_name.h"
#include "pinmux.h"
#include "aos/hal/i2c.h"
#include "string.h"

static aos_sem_t sem[6];
static aos_mutex_t mutex[6];

static void i2c_cb(int32_t idx, iic_event_e event)
{
    if (event == IIC_EVENT_TRANSFER_DONE) {
        aos_sem_signal(&sem[idx]);
    } else {
        printf("i2c err event %d\n", event);
    }
}

int32_t hal_i2c_init(i2c_dev_t *i2c)
{
    if (i2c == NULL) {
        return -1;
    }

    if (aos_sem_new(&sem[i2c->port], 1) != 0) {
        return -1;
    }

    if (aos_sem_wait(&sem[i2c->port], 100) != 0) {
        return -1;
    }

    if (aos_mutex_new(&mutex[i2c->port]) != 0) {
        return -1;
    }

    iic_handle_t handle = csi_iic_initialize(i2c->port, i2c_cb);
    if (handle == NULL) {
        return -1;
    }

    i2c->priv = (void *)handle;
    iic_mode_e mode = IIC_MODE_SLAVE;
    if (i2c->config.mode == I2C_MODE_MASTER) {
        mode = IIC_MODE_MASTER;
    }

    iic_speed_e speed = IIC_BUS_SPEED_STANDARD;
    if (i2c->config.freq == I2C_BUS_BIT_RATES_100K) {
        speed = IIC_BUS_SPEED_STANDARD;
    } else if (i2c->config.freq == I2C_BUS_BIT_RATES_400K) {
        speed = IIC_BUS_SPEED_FAST;
    } else if (i2c->config.freq == I2C_BUS_BIT_RATES_3400K) {
        speed = IIC_BUS_SPEED_HIGH;
    }

    iic_address_mode_e addr_mode = IIC_ADDRESS_10BIT;
    if (i2c->config.address_width == I2C_HAL_ADDRESS_WIDTH_7BIT) {
        addr_mode = IIC_ADDRESS_7BIT;
    }

    return  csi_iic_config(handle, mode, speed, addr_mode, i2c->config.dev_addr);
}

int32_t hal_i2c_master_send(i2c_dev_t *i2c, uint16_t dev_addr, const uint8_t *data,
                            uint16_t size, uint32_t timeout)
{
    aos_mutex_lock(&mutex[i2c->port], AOS_WAIT_FOREVER);
    int ret = csi_iic_master_send((iic_handle_t)i2c->priv, dev_addr, data, size, 0);
    ret = aos_sem_wait(&sem[i2c->port], timeout);
    if (ret != 0) {
        printf("i2c timeout\n");
    }
    aos_mutex_unlock(&mutex[i2c->port]);

    return ret;
}

int32_t hal_i2c_master_recv(i2c_dev_t *i2c, uint16_t dev_addr, uint8_t *data,
                            uint16_t size, uint32_t timeout)
{
    aos_mutex_lock(&mutex[i2c->port], AOS_WAIT_FOREVER);
    int ret = csi_iic_master_receive((iic_handle_t)i2c->priv, dev_addr, data, size, 0);
    aos_sem_wait(&sem[i2c->port], timeout);
    aos_mutex_unlock(&mutex[i2c->port]);

    return ret;
}

int32_t hal_i2c_slave_send(i2c_dev_t *i2c, const uint8_t *data, uint16_t size, uint32_t timeout)
{
    aos_mutex_lock(&mutex[i2c->port], AOS_WAIT_FOREVER);
    int ret = csi_iic_slave_send((iic_handle_t)i2c->priv, data, size);
    aos_sem_wait(&sem[i2c->port], timeout);
    aos_mutex_unlock(&mutex[i2c->port]);

    return ret;
}

int32_t hal_i2c_slave_recv(i2c_dev_t *i2c, uint8_t *data, uint16_t size, uint32_t timeout)
{
    aos_mutex_lock(&mutex[i2c->port], AOS_WAIT_FOREVER);
    int ret = csi_iic_slave_receive((iic_handle_t)i2c->priv, data, size);
    aos_sem_wait(&sem[i2c->port], timeout);
    aos_mutex_unlock(&mutex[i2c->port]);

    return ret;
}

int32_t hal_i2c_mem_write(i2c_dev_t *i2c, uint16_t dev_addr, uint16_t mem_addr,
                          uint16_t mem_addr_size, const uint8_t *data, uint16_t size,
                          uint32_t timeout)

{
    int ret  = 0;
    uint8_t *buf = malloc(mem_addr_size + size);

    if (mem_addr_size == I2C_MEM_ADDR_SIZE_8BIT) {
        buf[0] = (uint8_t)(mem_addr & 0xff);
    } else {
        buf[0] = (uint8_t)((mem_addr & 0xff00) >> 8);
        buf[1]= (uint8_t)(mem_addr & 0xff);
    }

    memcpy(&buf[mem_addr_size], data, size);

    aos_mutex_unlock(&mutex[i2c->port]);
    ret = csi_iic_master_send((iic_handle_t)i2c->priv, dev_addr, buf, mem_addr_size + size, 0);
    aos_sem_wait(&sem[i2c->port], timeout);
    aos_mutex_unlock(&mutex[i2c->port]);

    free(buf);

    return ret;
}

int32_t hal_i2c_mem_read(i2c_dev_t *i2c, uint16_t dev_addr, uint16_t mem_addr,
                         uint16_t mem_addr_size, uint8_t *data, uint16_t size,
                         uint32_t timeout)
{
    if (mem_addr_size == 1) {
         mem_addr &= 0xff;
    } else if (mem_addr_size == 2) {
        uint8_t temp = (uint8_t)((mem_addr & 0xff00) >> 8);
        mem_addr = (mem_addr & 0xff) << 8;
        mem_addr |= temp;
    } else {
        return -1;
    }

    aos_mutex_unlock(&mutex[i2c->port]);
    int ret = csi_iic_master_send((iic_handle_t)i2c->priv, dev_addr, &mem_addr, mem_addr_size, 0);
    if (ret != 0) {
        goto ERR;
    }

    ret = aos_sem_wait(&sem[i2c->port], timeout);
    if (ret != 0) {
        goto ERR;
    }

    ret = csi_iic_master_receive((iic_handle_t)i2c->priv, dev_addr, data, size, 0);
    if (ret != 0) {
        goto ERR;
    }

    ret = aos_sem_wait(&sem[i2c->port], timeout);
    if (ret != 0) {
        goto ERR;
    }

    return 0;

ERR:
    aos_mutex_unlock(&mutex[i2c->port]);
    return -1;
}

int32_t hal_i2c_finalize(i2c_dev_t *i2c)
{
    aos_sem_free(&sem[i2c->port]);
    return csi_iic_uninitialize((iic_handle_t)i2c->priv);
}
