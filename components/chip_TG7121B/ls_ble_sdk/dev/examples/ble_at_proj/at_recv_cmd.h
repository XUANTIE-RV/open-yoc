#ifndef _AT_RECV_CMD_H_
#define _AT_RECV_CMD_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"

enum
{
    AT_RECV_CMD,
    AT_RECV_TRANSPARENT_DATA,
    AT_TRANSPARENT_START_TIMER,
    RECV_CMD_MAX,
};

typedef struct _at_recv_data
{
    uint8_t *param;
    uint16_t param_len;
    uint8_t evt_id;
} at_recv_t;

void at_init(void);
void at_store_info_to_flash(void);
void at_load_info_from_flash(void);
void at_clr_uart_buff(void);
void uart_write(uint8_t *value, uint16_t length);
void uart_tx_it(uint8_t *value, uint16_t length);

#endif //_APP_AT_H
