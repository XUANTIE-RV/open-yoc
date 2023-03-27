/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef __APP_FACTORY_H
#define __APP_FACTORY_H

#include "stdint.h"

void user_at_gpio_handler(char *cmd, int type, char *data);
void user_at_z_handler(char *cmd, int type, char *data);
void user_at_psram_handler(char *cmd, int type, char *data);
void user_at_wjoin_handler(char *cmd, int type, char *data);
void user_at_wscan_handler(char *cmd, int type, char *data);

void app_ftmode_entry(void);
void app_ftsetting_restore();

#endif
