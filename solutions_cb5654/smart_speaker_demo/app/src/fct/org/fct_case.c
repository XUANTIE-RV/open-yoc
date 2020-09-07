#include <aos/aos.h>
#include <yoc/eventid.h>
#include <ctype.h>
#include <string.h>
#include "app_main.h"

#include <drv/adc.h>
#include <drv/gpio.h>
#include <pinmux.h>
#include <pin_name.h>
#include "silan_iomux.h"

#include <devices/devicelist.h>
#include <devices/wifi.h>
#include <devices/flash.h>
// #include <yoc/aui.h>
// #include "silan_voice_adc.h"

#include "fct.h"

#define TAG "fct_case"


static int get_fctstate(int isquery, int arg_num, char **args, char *test_result)
{
    if (isquery) {
        char fct_state[64] = {0};
        
        aos_kv_getstring("fct_test_state", fct_state, 64);

        strcpy(test_result, fct_state);
    } else if (arg_num == 1 && strlen(args[0]) <= 64) {
        char *fct_state = args[0];
        int ret;

        ret = aos_kv_setstring("fct_test_state", fct_state);
        if (ret != 0) {
            return FTEST_RESULT_ERR_FAILED;
        }
    } else {
        return FTEST_RESULT_ERR_PARAM;
    }

    return FTEST_RESULT_SUCCESS;
}

static int gpio_test(int isquery, int arg_num, char **args, char *test_result)
{
    if (!isquery && arg_num == 1 && args[0][0] == '0' && args[0][1] == 0) {
        
    } else {
        return FTEST_RESULT_ERR_PARAM;
    }

    return FTEST_RESULT_SUCCESS;
}

static int ram_test(int isquery, int arg_num, char **args, char *test_result)
{
    #define RAM_TEST_LEN    64

    int *sramsha_test_addr = (int *)0x01003000;
    int *sramsys_test_addr = (int *)0x20007000;
    char wtbytes[RAM_TEST_LEN], cpbytes[RAM_TEST_LEN];

    memset((char *)wtbytes, 0x11, RAM_TEST_LEN);

    if (!isquery && arg_num == 1 && args[0][0] == '0' && args[0][1] == 0) {
        memcpy(cpbytes, sramsha_test_addr, RAM_TEST_LEN);
        memcpy(sramsha_test_addr, wtbytes, RAM_TEST_LEN);
        if (memcmp(sramsha_test_addr, wtbytes, RAM_TEST_LEN) != 0) {
            return FTEST_RESULT_ERR_FAILED;
        }
        memcpy(sramsha_test_addr, cpbytes, RAM_TEST_LEN);

        memcpy(cpbytes, sramsys_test_addr, RAM_TEST_LEN);
        memcpy(sramsys_test_addr, wtbytes, RAM_TEST_LEN);
        if (memcmp(sramsys_test_addr, wtbytes, RAM_TEST_LEN) != 0) {
            return FTEST_RESULT_ERR_FAILED;
        }
        memcpy(sramsys_test_addr, cpbytes, RAM_TEST_LEN);
    } else {
        return FTEST_RESULT_ERR_PARAM;
    }

    return FTEST_RESULT_SUCCESS;
}

static int flash_test(int isquery, int arg_num, char **args, char *test_result)
{
    if (!isquery && arg_num == 1 && args[0][0] == '0' && args[0][1] == 0) {
        aos_dev_t *flash_dev = NULL;
        uint32_t flash_test_addr = 0x00687000;
        uint32_t wtbytes[8], rdbytes[8];
        int ret;

        flash_dev = flash_open("eflash0");
        CHECK_RET_TAG_WITH_RET(flash_dev != NULL, FTEST_RESULT_ERR_FAILED);

        ret = flash_erase(flash_dev, flash_test_addr, 1);
        CHECK_RET_TAG_WITH_RET(ret == 0, FTEST_RESULT_ERR_FAILED);

        memset((char *)wtbytes, 0x11, 8*sizeof(uint32_t));
        ret = flash_program(flash_dev, flash_test_addr, wtbytes, 8 * sizeof(uint32_t));
        CHECK_RET_TAG_WITH_RET(ret == 0, FTEST_RESULT_ERR_FAILED);

        ret = flash_read(flash_dev, flash_test_addr, rdbytes, 8 * sizeof(uint32_t));
        CHECK_RET_TAG_WITH_RET(ret == 0, FTEST_RESULT_ERR_FAILED);

        ret = memcmp(wtbytes, rdbytes, 8 * sizeof(uint32_t));
        CHECK_RET_TAG_WITH_RET(ret == 0, FTEST_RESULT_ERR_FAILED);
    } else {
        return FTEST_RESULT_ERR_PARAM;
    }

    return FTEST_RESULT_SUCCESS;
}

static int sdcard_test(int isquery, int arg_num, char **args, char *test_result)
{
    int ret;
    int fd = -1;

    if (!isquery && arg_num == 1 && args[0][0] == '0' && args[0][1] == 0) {
        const char *str = "Marry has a little sheep!";
        char rdbytes[128];
        fd = aos_open("/fatfs0/fct_test_file.txt", O_RDWR | O_CREAT | O_TRUNC);
        CHECK_RET_TAG_WITH_RET(fd >= 0, FTEST_RESULT_ERR_FAILED);
    
        ret = aos_write(fd, str, strlen(str));
        CHECK_RET_TAG_WITH_RET(ret == strlen(str), FTEST_RESULT_ERR_FAILED);

        aos_lseek(fd, 0, SEEK_SET);
        ret = aos_read(fd, rdbytes, strlen(str));
        CHECK_RET_TAG_WITH_RET(ret == strlen(str), FTEST_RESULT_ERR_FAILED);

        ret = memcmp(str, rdbytes, strlen(str));
        CHECK_RET_TAG_WITH_RET(ret == 0, FTEST_RESULT_ERR_FAILED);

        aos_close(fd);
    } else {
        return FTEST_RESULT_ERR_PARAM;
    }

    return FTEST_RESULT_SUCCESS;
}

static int adccal_test(int isquery, int arg_num, char **args, char *test_result)
{
    #define ADC_SAMPLE_ROUND    20

    if (!isquery && arg_num == 3) {
        int id = atoi(args[0]);
        int expect_val = atoi(args[1]);
        int max_offset = atoi(args[2]);

        if (id > 3 || id < 1 || max_offset < 10) {
            return FTEST_RESULT_ERR_PARAM;
        }

        int volts[ADC_SAMPLE_ROUND];
        int read_cnt = 0;
        int sum = 0;
        int avg;

        for (int i = 0; i < ADC_SAMPLE_ROUND; ++i) {
            if (app_adc_value_read(PA2, &volts[i])) {
                volts[i] = -1000;
                aos_msleep(5);
                continue;
            }

            sum += volts[i];
            read_cnt++;
            aos_msleep(5);
        }

        if (!read_cnt) {
            strncpy(test_result, "adc read err", FTEST_MAX_RESULT_LEN);
            return FTEST_RESULT_ERR_FAILED;
        }

        avg = sum / read_cnt;

        /* remove outliers */
        for (int i = 0; i < ADC_SAMPLE_ROUND; ++i) {
            if (volts[i] < 0) {
                continue;
            }

            if (abs(volts[i] - avg) > 20) {
                LOGD(TAG, "volt diff big %u %u", volts[i], avg);

                sum -= volts[i];
                read_cnt--;

            }
        }

        if (!read_cnt) {
            strncpy(test_result, "adc read bad", FTEST_MAX_RESULT_LEN);
            return FTEST_RESULT_ERR_FAILED;
        }

        avg = sum / read_cnt;

        if (abs(avg - expect_val) > max_offset) {
            return FTEST_RESULT_ERR_FAILED;
        }

        LOGD(TAG, "adc calibrate val %d", expect_val - avg);
        aos_kv_setint("adc_cal_offset", expect_val - avg);
    } else {
        return FTEST_RESULT_ERR_PARAM;
    }

    return FTEST_RESULT_SUCCESS;
}

static int low_power_test(int isquery, int arg_num, char **args, char *test_result)
{
    if (!isquery && arg_num == 1) {
        int mode = atoi(args[0]);

        if (mode < 1 || mode > 2) {
            return FTEST_RESULT_ERR_PARAM;
        }

        pm_config_policy(mode);
        pm_agree_halt(0);
    } else {
        return FTEST_RESULT_ERR_PARAM;
    }

    return FTEST_RESULT_SUCCESS;
}

static int mac_test(int isquery, int arg_num, char **args, char *test_result)
{
    if (isquery) {
        char mac_str[18] = {0};
        
        extern int wifi_get_mac_address(char *);
        int ret = wifi_get_mac_address(mac_str);
        CHECK_RET_WITH_RET(ret == 0, FTEST_RESULT_ERR_FAILED);

        strcpy(test_result, mac_str);
    } else if (arg_num == 1 && strlen(args[0]) == 17) {   // xx:xx:xx:xx:xx:xx
        char *mac = args[0];
        int n = 0;
        for (int i = 0; i < 17; ++i) {
            if (mac[i] == ':') {
                continue;
            }

            mac[n++] = mac[i];
        }
        mac[n] = 0;

        if (strlen(mac) != 12) {
            return FTEST_RESULT_ERR_PARAM;
        }

        extern int wifi_set_mac_address(char * mac);
        wifi_set_mac_address(mac);
    } else {
        return FTEST_RESULT_ERR_PARAM;
    }

    return FTEST_RESULT_SUCCESS;
}

static int alidevinfo_test(int isquery, int arg_num, char **args, char *test_result)
{
    #define ALI_INFO_MAX_LEN 64

    if (isquery) {
        char product_key[ALI_INFO_MAX_LEN] = {0};
        char product_secret[ALI_INFO_MAX_LEN] = {0};
        char device_name[ALI_INFO_MAX_LEN] = {0};
        char device_secret[ALI_INFO_MAX_LEN] = {0};
        char device_info[ALI_INFO_MAX_LEN * 4];
        
        aos_kv_getstring("hal_devinfo_pk", product_key, ALI_INFO_MAX_LEN);
        aos_kv_getstring("hal_devinfo_ps", product_secret, ALI_INFO_MAX_LEN);
        aos_kv_getstring("hal_devinfo_dn", device_name, ALI_INFO_MAX_LEN);
        aos_kv_getstring("hal_devinfo_ds", device_secret, ALI_INFO_MAX_LEN);

        snprintf(device_info, ALI_INFO_MAX_LEN * 4,"%s,%s,%s,%s", product_key, product_secret, device_name, device_secret);
        strcpy(test_result, device_info);
    } else if (arg_num == 4) {
        int ret;
        char *pk = args[0];
        char *ps = args[1];
        char *dn = args[2];
        char *ds = args[3];

        if (strlen(pk) > 0) {
            ret = aos_kv_setstring("hal_devinfo_pk", pk);
            CHECK_RET_WITH_RET(ret == 0, FTEST_RESULT_ERR_FAILED);
        }

        if (strlen(ps) > 0) {
            ret = aos_kv_setstring("hal_devinfo_ps", ps);
            CHECK_RET_WITH_RET(ret == 0, FTEST_RESULT_ERR_FAILED);
        }

        if (strlen(dn) > 0) {
            ret = aos_kv_setstring("hal_devinfo_dn", dn);
            CHECK_RET_WITH_RET(ret == 0, FTEST_RESULT_ERR_FAILED);
        }

        if (strlen(ds) > 0) {
            ret = aos_kv_setstring("hal_devinfo_ds", ds);
            CHECK_RET_WITH_RET(ret == 0, FTEST_RESULT_ERR_FAILED);
        }
    } else {
        return FTEST_RESULT_ERR_PARAM;
    }

    return FTEST_RESULT_SUCCESS;
}

static int rtc_test(int isquery, int arg_num, char **args, char *test_result)
{
    if (!isquery && arg_num == 1 && args[0][0] == '0' && args[0][1] == 0) {
        time_t time_before = rtc_to_system();
        aos_msleep(1020);
        time_t time_now = rtc_to_system();

        if (time_now <= time_before) {
            return FTEST_RESULT_ERR_FAILED;
        }
    } else {
        return FTEST_RESULT_ERR_PARAM;
    }

    return FTEST_RESULT_SUCCESS;
}

static int rfcal_test(int isquery, int arg_num, char **args, char *test_result)
{
    if (!isquery && arg_num == 1 && args[0][0] == '2' && args[0][1] == 0) {
        int ret;
        ret = aos_kv_setint("factory_wifi_test", 1);
        CHECK_RET_WITH_RET(ret == 0, FTEST_RESULT_ERR_FAILED);
    } else {
        return FTEST_RESULT_ERR_PARAM;
    }

    return FTEST_RESULT_SUCCESS;
}


void fct_case_init(void)
{
    utask_t *task = NULL;
    int wifi_test = 0;
    aos_kv_getint("factory_wifi_test", &wifi_test);
    if (wifi_test) {
        // extern void hal_wifi_test_enabled(int en);
        // hal_wifi_test_enabled(1);
        task = utask_new("at&cli", 6 * 1024, QUEUE_MSG_COUNT, AOS_DEFAULT_APP_PRI);
        CHECK_RET_WITH_GOTO(task != NULL, ERR);

        cli_service_init(task);

        // cli_reg_cmd_ifconfig_wifi();
        cli_reg_cmd_sysinfo();
        cli_reg_cmd_kvtool();

        while(1) aos_msleep(10000);
    } else {
        // aos_set_log_level(AOS_LL_ERROR);
        // console_init(2, 115200, 512);   // put 803 print to uart3
    }

    for (int i = 0; i < 5; i++) {
        printf("bonjour SC5654\r\n");
        aos_msleep(300);
    }

    task = utask_new("fct", 8 * 1024, 150, AOS_DEFAULT_APP_PRI + 5);
    CHECK_RET_WITH_GOTO(task, ERR);

    fct_init(1, 115200, task);

    fct_register_cmd("FCTSTAT",     NULL, get_fctstate);
    fct_register_cmd("GPIO",        NULL, gpio_test);
    fct_register_cmd("MAC",         NULL, mac_test);
    fct_register_cmd("ALIDEVINFO",  NULL, alidevinfo_test);
    fct_register_cmd("RAM",         NULL, ram_test);
    fct_register_cmd("RTC",         NULL, rtc_test);
    fct_register_cmd("FLASH",       NULL, flash_test);
    fct_register_cmd("SDCARD",      NULL, sdcard_test);
    fct_register_cmd("RFCAL",       NULL, rfcal_test);
    fct_register_cmd("ADCCAL",      NULL, adccal_test);
    fct_register_cmd("SLEEP",       NULL, low_power_test);

    while (1) aos_msleep(10000);
    
    return;

ERR:
    LOGD(TAG, "fct init failed");
}
