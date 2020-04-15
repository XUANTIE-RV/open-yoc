/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#ifndef _APP_MAIN_H_
#define  _APP_MAIN_H_

#include <stdbool.h>
#include <stdint.h>

extern void mdelay(uint32_t ms);

void shell_reg_cmd_gpio(void);
void shell_reg_cmd_pwm(void);
void shell_reg_cmd_timer(void);
void shell_reg_cmd_iic(void);


int usart_init(int32_t uart_id);
int32_t usart_send(const void *data, uint32_t num);
int32_t usart_receive(void *data, uint32_t num);
#endif
