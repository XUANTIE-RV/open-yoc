/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include "sha_test.h"

static csi_sha_t test_sha;
static uint32_t test_sha_idx;
static csi_sha_context_t test_sha_context;
static csi_sha_mode_t tst_sha_mode;
static uint8_t sha_init_flag = 0;
static uint8_t sha_uninit_flag = 0;
static uint8_t sha_started_flag = 0;

volatile uint8_t test_sha_cb_flag = 1;

char test_sha_str_buffer[MAX_SHA_TEST_SOURCE_LEN];
uint8_t test_sha_source_buffer[MAX_SHA_TEST_IN_BUFFER_LEN];
char test_sha_output_buffer[MAX_SHA_TEST_OUTPUT_BUFFER_LEN];

static void sha_event_cb_func(csi_sha_t *sha, csi_sha_event_t event, void *arg)
{
    if (SHA_EVENT_COMPLETE == event) {
        test_sha_cb_flag = 0;
    }
}


int test_set_sha_mode(char *args)
{
    sscanf(args, "%d", &tst_sha_mode);
    sha_init_flag = 0;
    TEST_CASE_TIPS("set sha mode as: %d", tst_sha_mode);
    return 0;
}


int test_sha_sync(char *args)
{
    uint32_t update_or_finish = 0;
    uint32_t data_len = 0 ;
    csi_error_t ret;
    uint32_t data_out_size;
    csi_sha_state_t state;

    memset(test_sha_str_buffer, 0, MAX_SHA_TEST_SOURCE_LEN);
    sscanf(args, "%u,%u,", &test_sha_idx, &update_or_finish);

    if (update_or_finish == 0) {
        sscanf(args, "%u,%u,%[^,],%u", &test_sha_idx, &update_or_finish, test_sha_str_buffer, &data_len);
    }

    hex_to_array(test_sha_str_buffer, data_len, test_sha_source_buffer);
    TEST_PRINT_HEX("SOURCE DATA:", test_sha_source_buffer, data_len);

    if (sha_init_flag == 0) {
        memset(&test_sha, 0, sizeof(test_sha));
        memset(&test_sha_context, 0, sizeof(test_sha_context));
        ret = csi_sha_init(&test_sha, test_sha_idx);
        TEST_CASE_ASSERT(ret == CSI_OK, "csi_sha_init error,csi_sha_init should return CSK_OK, but returned %d", ret);
        sha_init_flag = 1;
        sha_uninit_flag = 0;
    }

    if (sha_started_flag == 0) {
        ret = csi_sha_start(&test_sha, &test_sha_context, tst_sha_mode);
        TEST_CASE_ASSERT(ret == CSI_OK, "csi_sha_start error,csi_sha_start should return CSK_OK, but returned %d", ret);
        sha_started_flag = 1;
    }

    if (update_or_finish == 0) {
        ret = csi_sha_update(&test_sha,  &test_sha_context, test_sha_source_buffer, data_len);
        TEST_CASE_ASSERT(ret == CSI_OK, "csi_sha_update error,csi_sha_update should return CSK_OK, but returned %d", ret);

    } else {
        memset(test_sha_output_buffer, 0, MAX_SHA_TEST_OUTPUT_BUFFER_LEN);
        ret = csi_sha_finish(&test_sha, &test_sha_context, test_sha_output_buffer, &data_out_size);
        TEST_CASE_ASSERT(ret == CSI_OK, "csi_sha_finish error,csi_sha_finish should return CSK_OK, but returned %d", ret);
        TEST_PRINT_HEX("ENCRYPTED DATA:", test_sha_output_buffer, data_out_size);

        sha_started_flag = 0;

        if (sha_uninit_flag == 0) {
            csi_sha_uninit(&test_sha);
            sha_uninit_flag = 1;
        }
    }

    while (1) {
        ret = csi_sha_get_state(&test_sha, &state);
        TEST_CASE_ASSERT(ret == CSI_OK, "csi_sha_get_state error,csi_sha_get_state should return CSK_OK, but returned %d", ret);

        if (state.busy == 0) {
            break;
        }
    }

    return 0;
}


int test_sha_async(char *args)
{
    uint32_t update_or_finish = 0;
    uint32_t data_len = 0 ;
    csi_error_t ret;
    uint32_t data_out_size;
    csi_sha_state_t state;

    memset(test_sha_str_buffer, 0, MAX_SHA_TEST_SOURCE_LEN);
    sscanf(args, "%u,%u,", &test_sha_idx, &update_or_finish);

    if (update_or_finish == 0) {
        sscanf(args, "%u,%u,%[^,],%u", &test_sha_idx, &update_or_finish, test_sha_str_buffer, &data_len);
    }

    hex_to_array(test_sha_str_buffer, data_len, test_sha_source_buffer);
    TEST_PRINT_HEX("SOURCE DATA:", test_sha_source_buffer, data_len);

    if (sha_init_flag == 0) {
        ret = csi_sha_init(&test_sha, test_sha_idx);
        TEST_CASE_ASSERT(ret == CSI_OK, "csi_sha_init error,csi_sha_init should return CSK_OK, but returned %d", ret);
        sha_init_flag = 1;
        sha_uninit_flag = 0;
        ret = csi_sha_attach_callback(&test_sha, sha_event_cb_func, NULL);
        TEST_CASE_ASSERT(ret == CSI_OK, "csi_sha_attach_callback error,csi_sha_attach_callback should return CSK_OK, but returned %d", ret);
    }

    if (sha_started_flag == 0) {
        ret = csi_sha_start(&test_sha, &test_sha_context, tst_sha_mode);
        TEST_CASE_ASSERT(ret == CSI_OK, "csi_sha_start error,csi_sha_start should return CSK_OK, but returned %d", ret);
        sha_started_flag = 1;
    }

    if (update_or_finish == 0) {
        test_sha_cb_flag = 1;
        ret = csi_sha_update_async(&test_sha,  &test_sha_context, test_sha_source_buffer, data_len);
        TEST_CASE_ASSERT(ret == CSI_OK, "csi_sha_update error, csi_sha_update should return CSK_OK,csi_sha_update_async but returned %d", ret);

        while (test_sha_cb_flag);

    } else {
        memset(test_sha_output_buffer, 0, MAX_SHA_TEST_OUTPUT_BUFFER_LEN);
        ret = csi_sha_finish(&test_sha, &test_sha_context, test_sha_output_buffer, &data_out_size);
        TEST_CASE_ASSERT(ret == CSI_OK, "csi_sha_finish error,csi_sha_finish should return CSK_OK,csi_sha_finish but returned %d", ret);
        TEST_PRINT_HEX("ENCRYPTED DATA:", test_sha_output_buffer, data_out_size);
        sha_started_flag = 0;

        if (sha_uninit_flag == 0) {
            csi_sha_uninit(&test_sha);
            sha_uninit_flag = 1;
            csi_sha_detach_callback(&test_sha);
        }
    }

    while (1) {
        ret = csi_sha_get_state(&test_sha, &state);
        TEST_CASE_ASSERT(ret == CSI_OK, "csi_sha_get_state error,csi_sha_get_state should return CSK_OK, but returned %d", ret);

        if (state.busy == 0) {
            break;
        }
    }

    return 0;
}

int test_sha_sync_performance(char *args)
{
    uint32_t update_times = 0;
    uint32_t finish_times = 0;
    uint32_t test_loops = 0;
    uint32_t data_len = 0 ;
    csi_error_t ret;
    uint32_t data_out_size;

    uint32_t elapsed_ms = 0;
    float performance = 0.0;

    sscanf(args, "%u,%u,%u,%u,%u,%u", &test_sha_idx, &tst_sha_mode, &update_times, &finish_times, &test_loops, &data_len);

    ret = csi_sha_init(&test_sha, test_sha_idx);
    TEST_CASE_ASSERT(ret == CSI_OK, "csi_sha_init error,csi_sha_init should return CSK_OK, but returned %d", ret);
    memset(test_sha_output_buffer, 0, MAX_SHA_TEST_OUTPUT_BUFFER_LEN);

    generate_rand_array2(test_sha_source_buffer, 255, data_len);

    tst_timer_restart();

    for (uint32_t j = 0; j < test_loops; j++) {
        csi_sha_start(&test_sha, &test_sha_context, tst_sha_mode);

        for (uint32_t i = 0; i < update_times; i++) {
            ret = csi_sha_update(&test_sha,  &test_sha_context, test_sha_source_buffer, data_len);
            TEST_CASE_ASSERT(ret == CSI_OK, "csi_sha_update error, csi_sha_update should return CSK_OK, but returned %d", ret);
        }

        for (uint32_t i = 0; i < finish_times; i++) {
            csi_sha_finish(&test_sha, &test_sha_context, test_sha_output_buffer, &data_out_size);
            TEST_CASE_ASSERT(ret == CSI_OK, "csi_sha_finish error,csi_sha_finish should return CSK_OK, but returned %d", ret);
        }

        memset(test_sha_source_buffer, 0x00, data_len);
    }

    elapsed_ms = tst_timer_get_interval();
    performance = 1.0 * test_loops / elapsed_ms;
    TEST_CASE_TIPS("Keep data constant:test SHA sync performance: %f times/ms", performance);

    memset(test_sha_source_buffer, 0x00, data_len);

    tst_timer_restart();

    for (uint32_t j = 0; j < test_loops; j++) {
        generate_rand_array2(test_sha_source_buffer, 255, data_len);

        csi_sha_start(&test_sha, &test_sha_context, tst_sha_mode);

        for (uint32_t i = 0; i < update_times; i++) {
            ret = csi_sha_update(&test_sha,  &test_sha_context, test_sha_source_buffer, data_len);
            TEST_CASE_ASSERT(ret == CSI_OK, "csi_sha_update error, csi_sha_update should return CSK_OK, but returned %d", ret);
        }

        for (uint32_t i = 0; i < finish_times; i++) {
            ret = csi_sha_finish(&test_sha, &test_sha_context, test_sha_output_buffer, &data_out_size);
            TEST_CASE_ASSERT(ret == CSI_OK, "csi_sha_finish error,csi_sha_finish should return CSK_OK, but returned %d", ret);
        }

        memset(test_sha_source_buffer, 0x00, data_len);
    }

    elapsed_ms = tst_timer_get_interval();
    performance = 1.0 * test_loops / elapsed_ms;
    TEST_CASE_TIPS("Change data every loop:test SHA sync performance: %f times/ms", performance);

    csi_sha_uninit(&test_sha);

    return 0;
}


int test_sha_async_performance(char *args)
{
    uint32_t update_times = 0;
    uint32_t finish_times = 0;
    uint32_t test_loops = 0;
    uint32_t data_len = 0 ;
    csi_error_t ret;
    uint32_t data_out_size;

    uint32_t elapsed_ms = 0;
    float performance = 0.0;


    sscanf(args, "%u,%u,%u,%u,%u,%u", &test_sha_idx, &tst_sha_mode, &update_times, &finish_times, &test_loops, &data_len);

    ret = csi_sha_init(&test_sha, test_sha_idx);
    TEST_CASE_ASSERT(ret == CSI_OK, "csi_sha_init error,csi_sha_init should return CSK_OK, but returned %d", ret);
    ret = csi_sha_attach_callback(&test_sha, sha_event_cb_func, NULL);
    TEST_CASE_ASSERT(ret == CSI_OK, "csi_sha_attach_callback error, csi_sha_attach_callback should return CSK_OK, but returned %d", ret);
    memset(test_sha_output_buffer, 0, MAX_SHA_TEST_OUTPUT_BUFFER_LEN);

    generate_rand_array2(test_sha_source_buffer, 255, data_len);

    tst_timer_restart();

    for (uint32_t j = 0; j < test_loops; j++) {
        csi_sha_start(&test_sha, &test_sha_context, tst_sha_mode);

        for (uint32_t i = 0; i < update_times; i++) {
            test_sha_cb_flag = 1;
            ret = csi_sha_update_async(&test_sha,  &test_sha_context, test_sha_source_buffer, data_len);
            TEST_CASE_ASSERT(ret == CSI_OK, "csi_sha_update error, csi_sha_update should return CSK_OK, but returned %d", ret);

            while (test_sha_cb_flag);
        }

        for (uint32_t i = 0; i < finish_times; i++) {
            csi_sha_finish(&test_sha, &test_sha_context, test_sha_output_buffer, &data_out_size);
            TEST_CASE_ASSERT(ret == CSI_OK, "csi_sha_finish error,csi_sha_finish should return CSK_OK, but returned %d", ret);
        }
    }

    elapsed_ms = tst_timer_get_interval();
    performance = 1.0 * test_loops / elapsed_ms;
    TEST_CASE_TIPS("Keep data constant:test SHA async performance: %f times/ms", performance);

    memset(test_sha_source_buffer, 0x00, data_len);

    tst_timer_restart();

    for (uint32_t j = 0; j < test_loops; j++) {
        generate_rand_array2(test_sha_source_buffer, 255, data_len);

        csi_sha_start(&test_sha, &test_sha_context, tst_sha_mode);

        for (uint32_t i = 0; i < update_times; i++) {
            test_sha_cb_flag = 1;
            ret = csi_sha_update_async(&test_sha,  &test_sha_context, test_sha_source_buffer, data_len);
            TEST_CASE_ASSERT(ret == CSI_OK, "csi_sha_update error, csi_sha_update should return CSK_OK, but returned %d", ret);

            while (test_sha_cb_flag);
        }

        for (uint32_t i = 0; i < finish_times; i++) {
            ret = csi_sha_finish(&test_sha, &test_sha_context, test_sha_output_buffer, &data_out_size);
            TEST_CASE_ASSERT(ret == CSI_OK, "csi_sha_finish error,csi_sha_finish should return CSK_OK, but returned %d", ret);
        }

        memset(test_sha_source_buffer, 0x00, data_len);
    }

    elapsed_ms = tst_timer_get_interval();
    performance = 1.0 * test_loops / elapsed_ms;
    TEST_CASE_TIPS("Change data every loop:test SHA async performance: %f times/ms", performance);

    csi_sha_detach_callback(&test_sha);
    csi_sha_uninit(&test_sha);
    return 0;
}


int test_sha_sync_stability(char *args)
{
    uint32_t update_times = 0;
    uint32_t finish_times = 0;
    uint8_t rand_data = 0;
    uint32_t test_loops = 0;
    uint32_t data_len = 0 ;
    csi_error_t ret;
    uint32_t data_out_size;

    sscanf(args, "%u,%u,%u,%u,%u,%u,%u", &test_sha_idx, &tst_sha_mode, &rand_data, &update_times, &finish_times, &test_loops, &data_len);

    ret = csi_sha_init(&test_sha, test_sha_idx);
    TEST_CASE_ASSERT(ret == CSI_OK, "csi_sha_init error,csi_sha_init should return CSK_OK, but returned %d", ret);

    switch (rand_data) {
        case 0:
            generate_rand_array2(test_sha_source_buffer, 255, data_len);
            TEST_PRINT_HEX("SOURCE DATA:", test_sha_source_buffer, data_len);
            memset(test_sha_output_buffer, 0, MAX_SHA_TEST_OUTPUT_BUFFER_LEN);

            for (uint32_t j = 0; j < test_loops; j++) {
                csi_sha_start(&test_sha, &test_sha_context, tst_sha_mode);

                for (uint32_t i = 0; i < update_times; i++) {
                    ret = csi_sha_update(&test_sha,  &test_sha_context, test_sha_source_buffer, data_len);
                    TEST_CASE_ASSERT(ret == CSI_OK, "csi_sha_update error, csi_sha_update should return CSK_OK, but returned %d", ret);
                }

                for (uint32_t i = 0; i < finish_times; i++) {
                    csi_sha_finish(&test_sha, &test_sha_context, test_sha_output_buffer, &data_out_size);
                    TEST_CASE_ASSERT(ret == CSI_OK, "csi_sha_finish error,csi_sha_finish should return CSK_OK, but returned %d", ret);
                }

                TEST_PRINT_HEX("ENCRYPTED_DATA is:", test_sha_output_buffer, data_out_size);
            }

            break;

        case 1:
            for (uint32_t j = 0; j < test_loops; j++) {
                generate_rand_array2(test_sha_source_buffer, 255, data_len);
                TEST_PRINT_HEX("SOURCE DATA:", test_sha_source_buffer, data_len);
                memset(test_sha_output_buffer, 0, MAX_SHA_TEST_OUTPUT_BUFFER_LEN);
                csi_sha_start(&test_sha, &test_sha_context, tst_sha_mode);

                for (uint32_t i = 0; i < update_times; i++) {
                    ret = csi_sha_update(&test_sha,  &test_sha_context, test_sha_source_buffer, data_len);
                    TEST_CASE_ASSERT(ret == CSI_OK, "csi_sha_update error, csi_sha_update should return CSK_OK, but returned %d", ret);
                }

                for (uint32_t i = 0; i < finish_times; i++) {
                    csi_sha_finish(&test_sha, &test_sha_context, test_sha_output_buffer, &data_out_size);
                    TEST_CASE_ASSERT(ret == CSI_OK, "csi_sha_finish error,csi_sha_finish should return CSK_OK, but returned %d", ret);
                }

                TEST_PRINT_HEX("ENCRYPTED_DATA is:", test_sha_output_buffer, data_out_size);
            }

            break;

        default:
            TEST_CASE_ASSERT(1 == 0, "Rand data mode choose error!");
            break;
    }

    csi_sha_uninit(&test_sha);
    return 0;
}


int test_sha_async_stability(char *args)
{
    uint32_t update_times = 0;
    uint32_t finish_times = 0;
    uint8_t rand_data = 0;
    uint32_t test_loops = 0;
    uint32_t data_len = 0 ;
    csi_error_t ret;
    uint32_t data_out_size;

    sscanf(args, "%u,%u,%u,%u,%u,%u,%u", &test_sha_idx, &tst_sha_mode, &rand_data, &update_times, &finish_times, &test_loops, &data_len);

    ret = csi_sha_init(&test_sha, test_sha_idx);
    TEST_CASE_ASSERT(ret == CSI_OK, "csi_sha_init error,csi_sha_init should return CSK_OK, but returned %d", ret);

    ret = csi_sha_attach_callback(&test_sha, sha_event_cb_func, NULL);
    TEST_CASE_ASSERT(ret == CSI_OK, "csi_sha_attach_callback error, csi_sha_attach_callback should return CSK_OK, but returned %d", ret);

    switch (rand_data) {
        case 0:
            generate_rand_array2(test_sha_source_buffer, 255, data_len);
            TEST_PRINT_HEX("SOURCE DATA:", test_sha_source_buffer, data_len);
            memset(test_sha_output_buffer, 0, MAX_SHA_TEST_OUTPUT_BUFFER_LEN);

            for (uint32_t j = 0; j < test_loops; j++) {
                csi_sha_start(&test_sha, &test_sha_context, tst_sha_mode);

                for (uint32_t i = 0; i < update_times; i++) {
                    test_sha_cb_flag = 1;
                    ret = csi_sha_update_async(&test_sha,  &test_sha_context, test_sha_source_buffer, data_len);
                    TEST_CASE_ASSERT(ret == CSI_OK, "csi_sha_update error, csi_sha_update should return CSK_OK, but returned %d", ret);

                    while (test_sha_cb_flag);
                }

                for (uint32_t i = 0; i < finish_times; i++) {
                    test_sha_cb_flag = 1;
                    csi_sha_finish(&test_sha, &test_sha_context, test_sha_output_buffer, &data_out_size);
                    TEST_CASE_ASSERT(ret == CSI_OK, "csi_sha_finish error,csi_sha_finish should return CSK_OK, but returned %d", ret);

                    while (test_sha_cb_flag);
                }

                TEST_PRINT_HEX("ENCRYPTED_DATA is:", test_sha_output_buffer, data_out_size);
            }

            break;

        case 1:
            for (uint32_t j = 0; j < test_loops; j++) {
                generate_rand_array2(test_sha_source_buffer, 255, data_len);
                TEST_PRINT_HEX("SOURCE DATA:", test_sha_source_buffer, data_len);
                memset(test_sha_output_buffer, 0, MAX_SHA_TEST_OUTPUT_BUFFER_LEN);
                csi_sha_start(&test_sha, &test_sha_context, tst_sha_mode);

                for (uint32_t i = 0; i < update_times; i++) {
                    test_sha_cb_flag = 1;
                    ret = csi_sha_update_async(&test_sha,  &test_sha_context, test_sha_source_buffer, data_len);
                    TEST_CASE_ASSERT(ret == CSI_OK, "csi_sha_update error, csi_sha_update should return CSK_OK, but returned %d", ret);

                    while (test_sha_cb_flag);
                }

                for (uint32_t i = 0; i < finish_times; i++) {
                    test_sha_cb_flag = 1;
                    csi_sha_finish(&test_sha, &test_sha_context, test_sha_output_buffer, &data_out_size);
                    TEST_CASE_ASSERT(ret == CSI_OK, "csi_sha_finish error,csi_sha_finish should return CSK_OK, but returned %d", ret);

                    while (test_sha_cb_flag);
                }

                TEST_PRINT_HEX("ENCRYPTED_DATA is:", test_sha_output_buffer, data_out_size);
            }

            break;

        default:
            TEST_CASE_ASSERT(1 == 0, "Rand data mode choose error!\n");
            break;
    }

    csi_sha_detach_callback(&test_sha);
    csi_sha_uninit(&test_sha);

    return 0;
}