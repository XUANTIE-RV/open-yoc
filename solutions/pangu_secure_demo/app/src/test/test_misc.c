/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include "app_main.h"
#include "sec_crypto_aes.h"
#include "ulog/ulog.h"
#include <string.h>
#ifdef CONFIG_TEE_CA
#include "drv/tee.h"
#endif

void test_cid()
{
    uint32_t   ret;
    uint8_t    cid[512] = {0};
    uint32_t   cid_len  = 512;
#ifdef CONFIG_TEE_CA
    ret = csi_tee_rand_generate(cid, 4);
    if (ret) {
        printf("%s, %d, %x\n", __FUNCTION__, __LINE__, ret);
    }
    //aos_log_hexdump("random", (char*)cid, 4);

    ret = csi_tee_get_cid(cid, &cid_len);
    if (ret) {
        printf("%s, %d, %x\n", __FUNCTION__, __LINE__, ret);
    }
    printf("%s, %d, %x\n", __FUNCTION__, __LINE__, cid_len);
    aos_log_hexdump("cid", (char*)cid, cid_len);
#endif
}
