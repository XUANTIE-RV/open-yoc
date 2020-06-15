/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef DEVICE_IIC_PAI_H
#define DEVICE_IIC_PAI_H

#ifdef __cplusplus
extern "C" {
#endif

#include <devices/device.h>

/*----- IIC Control Codes: Mode -----*/
typedef enum {
    MODE_MASTER,             ///< IIC Master
    MODE_SLAVE               ///< IIC Slave
} hal_iic_mode_t;

/*----- IIC Control Codes: IIC Bus Speed -----*/
typedef enum {
    BUS_SPEED_STANDARD  = 0, ///< Standard Speed (100kHz)
    BUS_SPEED_FAST      = 1, ///< Fast Speed     (400kHz)
    BUS_SPEED_FAST_PLUS = 2, ///< Fast+ Speed    (  1MHz)
    BUS_SPEED_HIGH      = 3  ///< High Speed     (3.4MHz)
} hal_iic_speed_t;

/*----- IIC Control Codes: IIC Address Mode -----*/
typedef enum {
    ADDR_7BIT        = 0,  ///< 7-bit address mode
    ADDR_10BIT       = 1   ///< 10-bit address mode
} hal_iic_address_mode_t;

typedef struct {
    hal_iic_mode_t          mode;
    hal_iic_speed_t         speed;
    hal_iic_address_mode_t  addr_mode;
    int32_t                 slave_addr;
} iic_config_t;

#define I2C_MEM_ADDR_SIZE_8BIT  1 /* i2c menory address size 8bit */
#define I2C_MEM_ADDR_SIZE_16BIT 2 /* i2c menory address size 16bit */

#define iic_open(name) device_open(name)
#define iic_open_id(name, id) device_open_id(name, id)
#define iic_close(dev) device_close(dev)

int iic_config(aos_dev_t *dev, iic_config_t *config);
int iic_master_send(aos_dev_t *dev, uint16_t dev_addr, const void *data, uint32_t size, uint32_t timeout);
int iic_master_recv(aos_dev_t *dev, uint16_t dev_addr, void *data, uint32_t size, uint32_t timeout);
int iic_slave_send(aos_dev_t *dev, const void *data, uint32_t size, uint32_t timeout);
int iic_slave_recv(aos_dev_t *dev, void *data, uint32_t size, uint32_t timeout);
int iic_mem_write(aos_dev_t *dev, uint16_t dev_addr, uint16_t mem_addr, uint16_t mem_addr_size, const void *data, uint32_t size, uint32_t timeout);
int iic_mem_read(aos_dev_t *dev, uint16_t dev_addr, uint16_t mem_addr, uint16_t mem_addr_size, void *data, uint32_t size, uint32_t timeout);
void iic_config_default(iic_config_t *config);


#ifdef __cplusplus
}
#endif

#endif
