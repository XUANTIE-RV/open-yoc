/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include "mbox_test.h"

char testdata[MBOX_TEST_SIZE];
uint8_t rx_data[RX_SIZE];
volatile uint32_t receive_size = 0;
volatile uint8_t send_event_done = 0U;
volatile uint8_t send_done_flag = 0U;
volatile uint8_t recv_event_done = 0U;

static void mailbox_event_cb_send_fun(csi_mbox_t *mbox, csi_mbox_event_t event, uint32_t channel_id, uint32_t received_len, void *arg)
{
    if (event == MBOX_EVENT_SEND_COMPLETE) {
        send_event_done = 1;
    } else if (event == MBOX_EVENT_RECEIVED) {
        send_done_flag = 1;
    } else {
        TEST_CASE_TIPS("mailbox event error");
    }
}

static void mailbox_event_cb_receive_fun(csi_mbox_t *mbox, csi_mbox_event_t event, uint32_t channel_id, uint32_t received_len, void *arg)
{
    csi_error_t ret;
    int32_t memcmp_ret;

    if (event == MBOX_EVENT_SEND_COMPLETE) {
        recv_event_done = 1;
    } else if (event == MBOX_EVENT_RECEIVED) {
        memset(rx_data, 0, RX_SIZE);
        ret = csi_mbox_receive(mbox, channel_id, &rx_data, received_len);
        TEST_CASE_ASSERT(ret != CSI_ERROR, "mbox receive error");
        memcmp_ret = memcmp(testdata + receive_size, rx_data, ret);
        TEST_CASE_ASSERT(memcmp_ret == 0, "data sent is not equal to the received");
        receive_size += ret;
    } else {
        TEST_CASE_TIPS("mailbox event error");
    }
}

int test_mbox_transferSend(char *args)
{
    csi_error_t ret;
    csi_mbox_t mbox_handle;
    test_mbox_args_t r_mbox_args;
    uint32_t get_data[2];
    int32_t total_size = MBOX_TEST_SIZE;
    int32_t sent_size = 0;

    ret = args_parsing(args, get_data, 3);
    TEST_CASE_ASSERT_QUIT(ret == 0, "num_of_parameter error");

    r_mbox_args.idx = get_data[0];
    r_mbox_args.channel_id = get_data[1];
    r_mbox_args.trans_size = get_data[2];
    TEST_CASE_TIPS("mbox idx is %d, channel id is %d", r_mbox_args.idx, r_mbox_args.channel_id);

    ret = csi_mbox_init(&mbox_handle, r_mbox_args.idx);
    TEST_CASE_ASSERT(ret == CSI_OK, "mbox init error");

    ret = csi_mbox_attach_callback(&mbox_handle, mailbox_event_cb_send_fun, NULL);
    TEST_CASE_ASSERT(ret == CSI_OK, "mbox attach callback error");

    transfer_data(testdata, r_mbox_args.trans_size);

    send_event_done = 0;

    do {
        sent_size += csi_mbox_send(&mbox_handle, r_mbox_args.channel_id, testdata + sent_size, total_size - sent_size);

        while (!send_event_done);

        send_event_done = 0;
    } while (sent_size < total_size);

    while (!send_done_flag);

    csi_mbox_detach_callback(&mbox_handle);
    csi_mbox_uninit(&mbox_handle);
    return 0;
}

int test_mbox_transferReceive(char *args)
{
    csi_error_t ret;
    csi_mbox_t mbox_handle;
    test_mbox_args_t r_mbox_args;
    uint32_t get_data[3];

    ret = args_parsing(args, get_data, 3);
    TEST_CASE_ASSERT_QUIT(ret == 0, "num_of_parameter error");

    r_mbox_args.idx = get_data[0];
    r_mbox_args.channel_id = get_data[1];
    r_mbox_args.trans_size = get_data[2];
    TEST_CASE_TIPS("mbox idx is %d, channel id is %d", r_mbox_args.idx, r_mbox_args.channel_id);

    ret = csi_mbox_init(&mbox_handle, r_mbox_args.idx);
    TEST_CASE_ASSERT(ret == CSI_OK, "mbox init error");

    transfer_data(testdata, r_mbox_args.trans_size);

    ret = csi_mbox_attach_callback(&mbox_handle, mailbox_event_cb_receive_fun, NULL);
    TEST_CASE_ASSERT(ret == CSI_OK, "mbox attach callback error");

    TEST_CASE_READY();
    receive_size = 0;

    while (receive_size < MBOX_TEST_SIZE);

    while (!recv_event_done);

    csi_mbox_detach_callback(&mbox_handle);
    csi_mbox_uninit(&mbox_handle);

    return 0;
}