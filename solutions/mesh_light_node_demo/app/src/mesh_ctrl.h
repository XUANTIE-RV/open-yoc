/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef __MESH_CTRL_H
#define __MESH_CTRL_H

int mesh_dev_init(void);

int gen_onoff_set(uint8_t onoff, uint8_t is_ack);

#endif
