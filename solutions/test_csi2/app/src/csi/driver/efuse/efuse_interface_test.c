/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include "efuse_test.h"

int test_efuse_interface(char *args)
{
    csi_error_t ret;

    ret = drv_efuse_init(NULL, 0);
    TEST_CASE_ASSERT(ret == CSI_ERROR, "drv_efuse_init interface test error");

    ret = drv_efuse_read(NULL, 0, NULL, 0);
    TEST_CASE_ASSERT(ret == CSI_ERROR, "drv_efuse_read interface test error");

    ret = drv_efuse_program(NULL, 0, NULL, 0);
    TEST_CASE_ASSERT(ret == CSI_ERROR, "drv_efuse_program interface test error");

    ret = drv_efuse_get_info(NULL, NULL);
    TEST_CASE_ASSERT(ret == CSI_ERROR, "drv_efuse_get_info interface test error");

    drv_efuse_uninit(NULL);

    return 0;
}