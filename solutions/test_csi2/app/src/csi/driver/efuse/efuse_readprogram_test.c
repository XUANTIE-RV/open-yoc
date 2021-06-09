/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */


#include "efuse_test.h"


int test_efuse_readprogram(char *args)
{
    csi_efuse_info_t info;
    csi_error_t ret;
    csi_efuse_t efuse;
    uint32_t get_data[3];
    test_efuse_args_t efuse_args;
    uint8_t *tx_data = NULL;
    uint8_t *rx_data = NULL;

    ret = args_parsing(args, get_data, 3);
    TEST_CASE_ASSERT_QUIT(ret == 0, "num_of_parameter error");

    efuse_args.idx = get_data[0];
    efuse_args.addr = get_data[1];
    efuse_args.size = get_data[2];

    ret = drv_efuse_init(&efuse, 0);
    TEST_CASE_ASSERT(ret != CSI_ERROR, "efuse init error");

    ret = drv_efuse_get_info(&efuse, &info);
    TEST_CASE_ASSERT(ret != CSI_ERROR, "efuse get info error");
    TEST_CASE_TIPS("efuse info:satrt %d", info.start);
    TEST_CASE_TIPS("efuse info:end %d", info.end);

    tx_data = (uint8_t *)malloc(sizeof(uint8_t) * efuse_args.size);
    TEST_CASE_ASSERT(tx_data != NULL, "tx_data malloc error");
    rx_data = (uint8_t *)malloc(sizeof(uint8_t) * efuse_args.size);
    TEST_CASE_ASSERT(rx_data != NULL, "rx_data malloc error");

    memset(tx_data, 0xf, sizeof(uint8_t) * efuse_args.size);
    ret = drv_efuse_program(&efuse, efuse_args.addr, tx_data, efuse_args.size);
    TEST_CASE_ASSERT(ret == CSI_OK, "efuse program error");

    memset(rx_data, 0, sizeof(uint8_t) * efuse_args.size);
    ret = drv_efuse_read(&efuse, efuse_args.addr, rx_data, efuse_args.size);
    TEST_CASE_ASSERT(ret == CSI_OK, "efuse read error");
    ret = memcmp(tx_data, rx_data, efuse_args.size);
    TEST_CASE_ASSERT(ret == 0, "the result of efuse reading is not equal to that of efuse programing");

    memset(tx_data, 0x0, sizeof(uint8_t) * efuse_args.size);
    ret = drv_efuse_program(&efuse, efuse_args.addr, tx_data, efuse_args.size);
    TEST_CASE_ASSERT(ret == CSI_OK, "efuse program error");

    memset(rx_data, 0, sizeof(uint8_t) * efuse_args.size);
    ret = drv_efuse_read(&efuse, efuse_args.addr, rx_data, efuse_args.size);
    TEST_CASE_ASSERT(ret == CSI_OK, "efuse read error");

    memset(tx_data, 0xf, sizeof(uint8_t) * efuse_args.size);
    ret = memcmp(tx_data, rx_data, efuse_args.size);
    TEST_CASE_ASSERT(ret == 0, "the result of efuse reading is not equal to the data programed for the first time");

    free(tx_data);
    free(rx_data);

    drv_efuse_uninit(&efuse);

    return 0;
}
