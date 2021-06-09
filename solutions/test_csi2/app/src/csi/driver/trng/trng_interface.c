/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <trng_test.h>


int test_trng_interface(char *args)
{
    uint32_t ret = 0;

    ret = csi_rng_get_single_word(NULL);
    TEST_CASE_ASSERT_QUIT(ret == CSI_ERROR, "csi_rng_get_single_word interface test error,should return error, but return:%d", ret);

    ret = csi_rng_get_single_word(0);
    TEST_CASE_ASSERT_QUIT(ret == CSI_ERROR, "csi_rng_get_single_word interface test error,should return error, but return:%d", ret);

    ret = csi_rng_get_multi_word(NULL, 0);
    TEST_CASE_ASSERT(ret == CSI_ERROR, "csi_rng_get_multi_word interface test error,should return error, but return:%d", ret);

    ret = csi_rng_get_multi_word(NULL, 100);
    TEST_CASE_ASSERT(ret == CSI_ERROR, "csi_rng_get_multi_word interface test error, should return error, but return:%d", ret);

    return 0;
}
