/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <pwm_test.h>

static volatile csi_pwm_event_t get_event;
static volatile uint8_t get_ch;
static volatile uint32_t get_time;

static void pwm_callback(csi_pwm_t *pwm, csi_pwm_event_t event, uint8_t ch, uint32_t time_us, void *arg)
{
    get_event = event;
    get_ch = ch;
    get_time = time_us;
    (*(uint8_t *)arg) = 0;
}


int test_pwm_capture(char *args)
{
    csi_error_t ret_sta;

    int ret;
    test_pwm_args_t td;
    csi_pwm_t hd;
    uint32_t get_data[4];

    ret = args_parsing(args, get_data, 4);
    TEST_CASE_ASSERT_QUIT(ret == 0, "num_of_parameter error");


    td.dev_idx = (uint8_t)_dev_idx;
    td.channel = (uint8_t)get_data[0];
    td.capture_polarity = (uint8_t)get_data[1];
    td.capture_count = (uint32_t)get_data[2];
    td.delay_ms = (uint32_t)get_data[3];

    TEST_CASE_TIPS("test PWM idx is %d", td.dev_idx);
    TEST_CASE_TIPS("test pwm channel is %d", td.channel);
    TEST_CASE_TIPS("config capture polarity is %d", td.capture_polarity);
    TEST_CASE_TIPS("config capture count is %d", td.capture_count);
    TEST_CASE_TIPS("config signal received in %d ms", td.delay_ms);


    ret_sta = csi_pwm_init(&hd, td.dev_idx);
    TEST_CASE_ASSERT_QUIT(ret_sta == CSI_OK, "pwm %d init error,should return CSI_OK,but returned %d.", td.dev_idx, ret_sta);



    ret_sta = csi_pwm_capture_config(&hd, td.channel, td.capture_polarity, td.capture_count);

    if (ret_sta != 0) {
        csi_pwm_uninit(&hd);
        TEST_CASE_ASSERT_QUIT(1 == CSI_OK, "pwm %d capture config error,should return CSI_OK,but returned %d.", td.dev_idx, ret_sta);
    }

    volatile uint8_t event_lock;

    event_lock = 1;
    get_ch = 0;
    get_time = 0;
    ret_sta = csi_pwm_attach_callback(&hd, pwm_callback, (void *)&event_lock);

    if (ret_sta != 0) {
        csi_pwm_uninit(&hd);
        TEST_CASE_ASSERT_QUIT(1 == CSI_OK, "pwm %d attach callback error,should return CSI_OK,but returned %d.", td.dev_idx, ret_sta);
    }

    TEST_CASE_READY();
    ret_sta = csi_pwm_capture_start(&hd, td.channel);

    if (ret_sta != 0) {
        csi_pwm_uninit(&hd);
        TEST_CASE_ASSERT_QUIT(1 == CSI_OK, "pwm %d capture start error,should return CSI_OK,but returned %d.", td.dev_idx, ret_sta);
    }

    while (event_lock) ;

    csi_pwm_capture_stop(&hd, td.channel);

    csi_pwm_detach_callback(&hd);

    TEST_CASE_ASSERT(get_event == td.capture_polarity, "callback get event error,should return %d ,but returned %d", td.capture_polarity, get_event);

    TEST_CASE_ASSERT(get_ch == td.channel, "callback get channel error,should return %d ,but returned %d", td.channel, get_ch);

    if ((get_time > (td.delay_ms + td.capture_count * 10) * 1000) || (get_time == 0)) {
        TEST_CASE_ASSERT(1 == 0, "callback get capture time error, expected_time is %dms,but actual_time is %dms", td.delay_ms, get_time / 1000);
    }

    csi_pwm_uninit(&hd);
    return 0;
}
