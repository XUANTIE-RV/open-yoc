#include "aos/hal/i2c.h"
#include <stddef.h>
#include <stdio.h>
#include <aos/kernel.h>
#include "field_manipulate.h"
#include "reg_i2c.h"
#include "reg_i2c_type.h"
#include "le501x_hal_i2c.h"
#include "reg_rcc_type.h"
#include "reg_rcc.h"

I2C_HandleTypeDef I2Cx_Handle;

#define I2C_MEM_ADD_MSB(__ADDRESS__)              ((uint8_t)((uint16_t)(((uint16_t)((__ADDRESS__) & (uint16_t)(0xFF00U))) >> 8U)))
#define I2C_MEM_ADD_LSB(__ADDRESS__)              ((uint8_t)((uint16_t)((__ADDRESS__) & (uint16_t)(0x00FFU))))

#define I2C_ADDRESS 0x27 //Test Addr

int32_t hal_i2c_init(i2c_dev_t *i2c)
{
    if (NULL == i2c) {
        printf("input param invalid in %s \r\n", __func__);
        return -1;
    }

    switch(i2c->port) {
    case 0:
        REG_FIELD_WR(RCC->APB1EN, RCC_I2C1, 1);
        I2Cx_Handle.Instance = I2C1;
        break;
    case 1:
        REG_FIELD_WR(RCC->APB1EN, RCC_I2C2, 1);
        I2Cx_Handle.Instance  = I2C2;
        break;
    default:
        printf("invalid i2c port in %s \r\n", __func__);
        break;
    }

    //I2Cx_Handle.Init.ClockSpeed      = 100000;
    I2Cx_Handle.Init.ClockSpeed      = i2c->config.freq;
    I2Cx_Handle.Init.OwnAddress1     = I2C_ADDRESS;
    //I2Cx_Handle.Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT;
    I2Cx_Handle.Init.AddressingMode  = i2c->config.address_width;
    I2Cx_Handle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    I2Cx_Handle.Init.OwnAddress2     = 0xFF;
    I2Cx_Handle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    I2Cx_Handle.Init.NoStretchMode   = I2C_NOSTRETCH_DISABLE;
    HAL_I2C_Init(&I2Cx_Handle);

    return 0;
}

int32_t hal_i2c_master_send(i2c_dev_t *i2c, uint16_t dev_addr, const uint8_t *data,
                            uint16_t size, uint32_t timeout)
{
    if (NULL == i2c || NULL == data ) {
        printf("input param invalid in %s \r\n", __func__);
        return -1;
    }

    HAL_I2C_Master_Transmit(&I2Cx_Handle, dev_addr, (void *)data, size, timeout);
    return 0;
}

int32_t hal_i2c_master_recv(i2c_dev_t *i2c, uint16_t dev_addr, uint8_t *data,
                            uint16_t size, uint32_t timeout)
{
    if (NULL == i2c || NULL == data ) {
        printf("input param invalid in %s \r\n", __func__);
        return -1;
    }

    HAL_I2C_Master_Receive(&I2Cx_Handle, dev_addr, data, size, timeout);
    return 0;
}

int32_t hal_i2c_slave_send(i2c_dev_t *i2c, const uint8_t *data, uint16_t size, uint32_t timeout);


int32_t hal_i2c_slave_recv(i2c_dev_t *i2c, uint8_t *data, uint16_t size, uint32_t timeout);

int32_t hal_i2c_mem_write(i2c_dev_t *i2c, uint16_t dev_addr, uint16_t mem_addr,
                          uint16_t mem_addr_size, const uint8_t *data, uint16_t size,
                          uint32_t timeout)
{
    size_t writecount = 0;
    uint8_t *pwritebuf = NULL;

    if (NULL == i2c || NULL == data || size == 0
        || ((mem_addr_size != I2C_MEM_ADDR_SIZE_8BIT) && (mem_addr_size != I2C_MEM_ADDR_SIZE_16BIT))) {
        printf("input param invalid in %s \r\n", __func__);
        return -1;
    }

    writecount = size + mem_addr_size;
    pwritebuf = aos_malloc(writecount);
    if (NULL == pwritebuf) {
        printf("i2c master mem write fail for i2c memory %d malloc fail\r\n", writecount);
        return -1;
    }

    if (mem_addr_size == I2C_MEM_ADDR_SIZE_8BIT) {
        pwritebuf[0] = I2C_MEM_ADD_LSB(mem_addr);
    } else if (mem_addr_size == I2C_MEM_ADDR_SIZE_16BIT) {
        pwritebuf[0] = I2C_MEM_ADD_MSB(mem_addr);
        pwritebuf[1] = I2C_MEM_ADD_LSB(mem_addr);
    }
    memcpy(pwritebuf + mem_addr_size, data, size);

    HAL_I2C_Master_Transmit(&I2Cx_Handle, dev_addr, pwritebuf, writecount, timeout);
    aos_msleep(100);
    aos_free(pwritebuf);

    return 0;
}

int32_t hal_i2c_mem_read(i2c_dev_t *i2c, uint16_t dev_addr, uint16_t mem_addr,
                         uint16_t mem_addr_size, uint8_t *data, uint16_t size,
                         uint32_t timeout)
{
    uint8_t mem_address[2] = {0};

    if (NULL == i2c || NULL == data || size == 0
        || ((mem_addr_size != I2C_MEM_ADDR_SIZE_8BIT) && (mem_addr_size != I2C_MEM_ADDR_SIZE_16BIT))) {
        printf("input param invalid in %s \r\n", __func__);
        return -1;
    }

    if (mem_addr_size == I2C_MEM_ADDR_SIZE_8BIT) {
        mem_address[0] = I2C_MEM_ADD_LSB(mem_addr);
    } else if (mem_addr_size == I2C_MEM_ADDR_SIZE_16BIT) {
        mem_address[0] = I2C_MEM_ADD_MSB(mem_addr);
        mem_address[1] = I2C_MEM_ADD_LSB(mem_addr);
    }

    HAL_I2C_Master_Transmit(&I2Cx_Handle, dev_addr, mem_address, mem_addr_size, timeout);
    aos_msleep(100);  // ??
    HAL_I2C_Master_Receive(&I2Cx_Handle, dev_addr, data, size, timeout);

    return 0;
}

int32_t hal_i2c_finalize(i2c_dev_t *i2c)
{
    if (NULL == i2c) {
        printf("input param invalid in %s \r\n", __func__);
        return -1;
    }

    switch(i2c->port) {
    case 0:
        REG_FIELD_WR(RCC->APB1EN, RCC_I2C1, 0);
        break;
    case 1:
        REG_FIELD_WR(RCC->APB1EN, RCC_I2C2, 0);
        break;
    default:
        printf("invalid i2c port in %s \r\n", __func__);
        break;
    }

    return 0;
}