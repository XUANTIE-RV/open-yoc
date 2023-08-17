/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Register ec200a 4G module rndis driver
 * @return      void
 */
void drv_ec200a_rndis_register();


/**
 * Register ec200a 4G module usb serial AT command driver
 * @return      void
 */
void drv_ec200a_serial_register(uint8_t idx);


/**
 * Register usb msc driver
 * @return      void
 */
void rvm_usb_msc_drv_register(int idx);

#ifdef __cplusplus
}
#endif