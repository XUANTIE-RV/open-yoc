/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <aos/aos.h>

#include <misc/printk.h>
#include <misc/byteorder.h>
#include <tinycrypt/sha256.h>
#include <tinycrypt/constants.h>
//#include <port/mesh_hal_sec.h>

#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_MESH_DEBUG_MODEL)
#include "common/log.h"
#include "crc16.h"
#include "genie_service.h"

static genie_triple_t genie_triple;

genie_storage_status_e genie_triple_write(uint32_t *p_pid, uint8_t *p_mac, uint8_t *p_key)
{
    uint16_t triple_crc = 0;
    uint16_t triple_crc_magic = GENIE_TRIPLE_CRC_MAGIC;
    uint8_t data[GENIE_SIZE_TRI_TRUPLE];

    memcpy(data, p_pid, GENIE_TRIPLE_PID_SIZE);
    memcpy(data + GENIE_TRIPLE_PID_SIZE, p_key, GENIE_TRIPLE_KEY_SIZE);
    memcpy(data + GENIE_TRIPLE_PID_SIZE + GENIE_TRIPLE_KEY_SIZE, p_mac, GENIE_TRIPLE_MAC_SIZE);

    triple_crc = util_crc16_compute(data, GENIE_SIZE_TRI_TRUPLE - GENIE_TRIPLE_CRC_LEN, &triple_crc_magic);
    memcpy(data + GENIE_TRIPLE_PID_SIZE + GENIE_TRIPLE_KEY_SIZE + GENIE_TRIPLE_MAC_SIZE, (unsigned char *)&triple_crc, GENIE_TRIPLE_CRC_LEN);

    return genie_storage_write_reliable(GFI_MESH_TRITUPLE, data, GENIE_SIZE_TRI_TRUPLE);
}

genie_storage_status_e genie_triple_read(uint32_t *p_pid, uint8_t *p_mac, uint8_t *p_key)
{
    uint16_t triple_crc = 0;
    uint16_t triple_read_crc = 0;
    uint16_t triple_crc_magic = GENIE_TRIPLE_CRC_MAGIC;
    unsigned char *p_data = NULL;
    genie_storage_status_e ret;
    uint8_t data[GENIE_SIZE_TRI_TRUPLE];

    ret = genie_storage_read_reliable(GFI_MESH_TRITUPLE, data, GENIE_SIZE_TRI_TRUPLE);
    if (GENIE_STORAGE_SUCCESS != ret)
    {
        GENIE_LOG_ERR("read triple fail:%d", ret);
        return ret;
    }

    triple_crc = util_crc16_compute(data, GENIE_SIZE_TRI_TRUPLE - GENIE_TRIPLE_CRC_LEN, &triple_crc_magic);

    p_data = data + GENIE_TRIPLE_PID_SIZE + GENIE_TRIPLE_KEY_SIZE + GENIE_TRIPLE_MAC_SIZE;

    triple_read_crc = p_data[0] | (p_data[1] << 8);
    if (triple_crc != triple_read_crc)
    {
        return GENIE_STORAGE_READ_FAIL;
    }

    memcpy(p_pid, data, GENIE_TRIPLE_PID_SIZE);
    memcpy(p_key, data + GENIE_TRIPLE_PID_SIZE, GENIE_TRIPLE_KEY_SIZE);
    memcpy(p_mac, data + GENIE_TRIPLE_PID_SIZE + GENIE_TRIPLE_KEY_SIZE, GENIE_TRIPLE_MAC_SIZE);

    return GENIE_STORAGE_SUCCESS;
}

int8_t genie_triple_init(void)
{
    genie_storage_status_e ret;

    memset(&genie_triple, 0, sizeof(genie_triple_t));

    ret = genie_triple_read(&genie_triple.pid, genie_triple.mac, genie_triple.key);
    if (ret != GENIE_STORAGE_SUCCESS)
    {
        GENIE_LOG_ERR("No genie triples,please burn them");
        return -1;
    }

    return 0;
}

genie_triple_t *genie_triple_get(void)
{
    return &genie_triple;
}

uint8_t *genie_triple_get_mac(void)
{
    return genie_triple.mac;
}
