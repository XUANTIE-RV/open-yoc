
/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     wj_usi_ll.h
 * @brief    header file for usi ll driver
 * @version  V1.0
 * @date     1. Mon 2020
 ******************************************************************************/

#ifndef __WJ_USI_COM_H__
#define __WJ_USI_COM_H__

#include <stdio.h>
#include <soc.h>
#include <csi_core.h>
#include <drv/spi.h>
#include <drv/iic.h>
#include <drv/uart.h>
#include <drv/common.h>
extern const uint16_t usi_rx_hs_num[] ;
extern const uint16_t usi_tx_hs_num[] ;
extern void wj_usi_irq_handler(void *arg);
extern void ck_usi_uart_irqhandler(csi_uart_t *uart);
extern void ck_usi_spi_irqhandler(csi_spi_t   *spi);
extern void ck_usi_i2c_irqhandler(csi_iic_t   *iic);
extern uint32_t usi_find_max_prime_num(uint32_t num);
#ifdef CONFIG_PM
extern csi_error_t usi_pm_action(csi_dev_t *dev, csi_pm_dev_action_t action);
csi_error_t usi_enable_pm(csi_dev_t *dev);
void usi_disable_pm(csi_dev_t *dev);
#endif
#endif ///__WJ_USI_COM_H__


