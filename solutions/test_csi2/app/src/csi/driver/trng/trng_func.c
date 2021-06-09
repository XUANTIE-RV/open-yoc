/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <trng_test.h>

uint32_t ndata[TRNG_DATA_LEN] = {0};

int test_trng_multi(char *args)
{
    uint32_t ret = 0;
    uint32_t number_of_words = 0;
    uint32_t number_of_loop = 0;
    uint32_t parameterms[2];
    int ret_args = 0;

    ret_args = args_parsing(args, parameterms, 2);
    TEST_CASE_ASSERT_QUIT(ret_args == 0, "num_of_parameter error");

    number_of_words = parameterms[0];
    number_of_loop = parameterms[1];

    uint32_t i, j;

    for (i = 1; i <= number_of_loop; i++) {
        ret = csi_rng_get_multi_word(ndata, number_of_words);
        TEST_CASE_ASSERT_QUIT(ret == CSI_OK, "get trng_get_multi_word error");

        atserver_send("\r\ndata:%d[", i);

        for (j = 1; j <= number_of_words; j++) {
            atserver_send("%u,", ndata[j - 1]);
        }

        atserver_send("]");
    }

    return 0;
}

int test_trng_single(char *args)
{
    uint32_t ret = 0;
    uint32_t data = 0;
    uint32_t number_of_loop = 0;
    uint32_t parameterms[1];
    int ret_args = 0;

    ret_args = args_parsing(args, parameterms, 1);
    TEST_CASE_ASSERT_QUIT(ret_args == 0, "num_of_parameter error");

    number_of_loop = parameterms[0];

    uint32_t i;

    for (i = 1; i <= number_of_loop; i++) {
        ret = csi_rng_get_single_word(&data);
        TEST_CASE_ASSERT_QUIT(ret == CSI_OK, "get trng_get_single_word error");

        atserver_send("%u,", data);
    }

    return 0;
}


int test_trng_single_performance(char *args)
{
    uint32_t data = 0;
    uint32_t ret = 0;
    uint32_t number_of_loop = 0;
    uint32_t parameterms[1];
    uint32_t time_ms;
    float performance;
    int ret_args = 0;
    ret_args = args_parsing(args, parameterms, 1);
    TEST_CASE_ASSERT_QUIT(ret_args == 0, "num_of_parameter error");
    number_of_loop = parameterms[0];
    uint32_t i;
    tst_timer_restart();

    for (i = 1; i <= number_of_loop; i++) {
        ret = csi_rng_get_single_word(&data);
        TEST_CASE_ASSERT_QUIT(ret == CSI_OK, "get trng_get_single_word error");
    }

    time_ms = tst_timer_get_interval();
    performance = (1.0 * number_of_loop) / time_ms;

    TEST_CASE_TIPS("TRNG single performance: %f times/ms", performance);

    return 0;
}


int test_trng_multi_performance(char *args)
{
    uint32_t ret = 0;
    uint32_t number_of_words = 0;
    uint32_t number_of_loop = 0;
    uint32_t time_ms;
    float performance;

    uint32_t parameterms[2];
    int ret_args = 0;

    ret_args = args_parsing(args, parameterms, 2);
    TEST_CASE_ASSERT_QUIT(ret_args == 0, "num_of_parameter error");

    number_of_words = parameterms[0];
    number_of_loop = parameterms[1];

    uint32_t i;
    tst_timer_restart();

    for (i = 1; i <= number_of_loop; i++) {
        ret = csi_rng_get_multi_word(ndata, number_of_words);
        TEST_CASE_ASSERT_QUIT(ret == CSI_OK, "get trng_get_multi_word error");
    }

    time_ms = tst_timer_get_interval();
    performance = (1.0 * number_of_loop) / time_ms;

    TEST_CASE_TIPS("TRNG multi performance: %f times/ms", performance);

    return 0;
}
