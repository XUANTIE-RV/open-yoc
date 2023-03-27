/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */
#ifndef _MESH_OCC_AUTH_H
#define _MESH_OCC_AUTH_H

#define OCC_AUTH_MESH_OOB_SIZE 8

int occ_auth_init(void);
int occ_auth_set_dev_mac_by_kp(void);
int occ_auth_get_uuid_and_oob(uint8_t uuid[16], uint32_t *short_oob);

#endif
