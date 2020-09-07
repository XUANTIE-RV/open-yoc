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
#include <yoc/record.h>
#include <yoc/mic.h>
#include <yv_ap.h>
// #include <yoc/aui.h>
#include <media.h>
// #include "silan_voice_adc.h"

#include "fct.h"

#define TAG "fct_case"

static int test_fctstate(int isquery, int arg_num, char **args, char *test_result)
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

static int test_passed(int isquery, int arg_num, char **args, char *test_result)
{
    if (!isquery && arg_num == 1 && args[0][0] == '0' && args[0][1] == 0) {
        int ret;

        ret = aos_kv_del("factory_test_mode");
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

static int key_test(int isquery, int arg_num, char **args, char *test_result)
{
    #define KEY_READ_TIME   10

    int ret;

    if (!isquery && arg_num == 1) {
        int key_id = atoi(args[0]);
#if (defined(BOARD_MIT_V2) || defined (BOARD_MIT_V3))
        int adc_pin[6] = {PIN_ADC_KEY, PIN_ADC_KEY, PIN_ADC_KEY, PIN_ADC_KEY2, PIN_ADC_KEY2, PIN_ADC_KEY2};
        int adc_vol[6] = {KEY_ADC_VAL1, KEY_ADC_VAL2, KEY_ADC_VAL3, KEY_ADC_VAL5, KEY_ADC_VAL6, KEY_ADC_VAL7};
#else
        int adc_pin[6] = {PIN_ADC_KEY, PIN_ADC_KEY, PIN_ADC_KEY, PIN_ADC_KEY, PIN_ADC_KEY};
        int adc_vol[6] = {KEY_ADC_VAL1, KEY_ADC_VAL2, KEY_ADC_VAL3, KEY_ADC_VAL4, KEY_ADC_VAL5};
#endif
        int key_state;
        int adc_key_num = sizeof(adc_pin) / sizeof(int);

        if (key_id <= 0 || key_id > adc_key_num + 1) {
            return FTEST_RESULT_ERR_PARAM;   
        }

        if (key_id == (sizeof(adc_pin) / sizeof(int)) + 1) {      // id 7: touch pad
#ifdef PIN_TOUCH_PAD
            int val = 0;
            app_gpio_read(PIN_TOUCH_PAD, &val);
            key_state = val ? 0 : 1;
#else
            return FTEST_RESULT_ERR_PARAM;
#endif
        } else {
            int vol;

            for (int i = 0; i < 3; ++i) {
                ret = app_adc_value_read(adc_pin[key_id-1], &vol);
                if (ret < 0) {
                    LOGE(TAG, "vol read err");
                    if (i == 2) {
                        return FTEST_RESULT_ERR_FAILED;
                    }
                    continue;
                } else {
                    key_state = (abs(vol - adc_vol[key_id-1]) <= KEY_AD_VAL_OFFSET) ? 1 : 0;
                    break;
                }
            }
        }

        char result[2] = {0};
        result[0] = '0' + key_state;
        strcpy(test_result, result);
    } else {
        return FTEST_RESULT_ERR_PARAM;
    }

    return FTEST_RESULT_SUCCESS;
}

static int led_test(int isquery, int arg_num, char **args, char *test_result)
{
    if (!isquery && arg_num == 1 && args[0][0] == '0' && args[0][1] == 0) {
#if (defined(BOARD_MIT_V2) || defined (BOARD_MIT_V3))
        int ret;
        for (int i = LED_ARRAY_1; i <= LED_ARRAY_12; ++i) {
            ret = aw9523_led_control(&g_i2c_dev, i, 100);
            CHECK_RET_WITH_RET(ret == 0, FTEST_RESULT_ERR_FAILED);
        }

        aw2013_led_control(&g_i2c_dev, LED_GREEN, LED_FLASH, 128);
        CHECK_RET_WITH_RET(ret == 0, FTEST_RESULT_ERR_FAILED);
#else
        return FTEST_RESULT_ERR_PARAM;
#endif
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

#define CHUNK_SIZE (5 * 3200)
#define TMP_RECORD_FILE  "/fatfs0/tmp_record.pcm"

#define EVT_MEDIA_FINISHED      (1 << 0)
#define EVT_MEDIA_ERROR         (1 << 1)

static aos_event_t evt_media_stat;
static rec_hdl_t rec_hdl;
static int print_period = 0;
static int filefd = -1;

extern void yv_debug_hook(mic_event_t hook, int dbg_level);

static void mic_copy_data(uint8_t *data, int size)
{
    if (filefd < 0) {
        return;
    }

    if (print_period % (320 * 1000) == 0) {
        LOGD(TAG, ".");
    }
    print_period += size;

    int ret = aos_write(filefd, data, size);
    if (size != ret) {
        LOGW(TAG, "sd write err, write=%d ret=%d", size, ret);
    }
}

static int pcm_send_start(char *ip, char *port, uint32_t *total_size)
{
    char buf[64];
    char   buf2[128];
    const char *filepath = TMP_RECORD_FILE;
    struct stat fstat;
    time_t tt = time(NULL);
    char ws_uri[128];
    int ret;

    ret = aos_stat(filepath, &fstat);
    CHECK_RET_WITH_RET(ret == 0, -1);
    *total_size = fstat.st_size;

    snprintf(ws_uri, 128, "ws://%s:%s", ip, port);
    snprintf(buf, sizeof(buf), "sdbuffer://handle=%s", filepath);
    snprintf(buf2, sizeof(buf2), "%s/%d_%s_%s", ws_uri, tt, "mic", "output.pcm");
    
    rec_hdl = record_register(buf, buf2);
    CHECK_RET_TAG_WITH_RET(rec_hdl, -1);

    // record_set_data_ready_cb(rec_hdl, data_ready, NULL);
    record_set_chunk_size(rec_hdl, CHUNK_SIZE);
    ret = record_start(rec_hdl);
    CHECK_RET_WITH_RET(ret == 0, -1);

    LOGD(TAG, "start mic rec\n");

    print_period = 0;
    return 0;
}

static int pcm_send_stop(void)
{
    int ret;

    if (rec_hdl == NULL) {
        return -1;
    }

    ret = record_stop(rec_hdl);
    CHECK_RET_WITH_RET(ret == 0, -1);

    record_unregister(rec_hdl);
    rec_hdl = NULL;

    aos_close(filefd);
    filefd = -1;

    LOGD(TAG, "mic rec stop over.");
    
    return 0;
}

static void mic_event_cb(void *priv, mic_event_id_t evt_id, void *data, int len)
{
    static uint16_t pcm_buf[3200 / 2];


    switch (evt_id)
    {
        case YV_PCM_EVT:
            if (len != 3200 && len != 2560) {
                LOGE(TAG,"write_ws %d", len);
                return;
            }

            // if (!g_pcm_buf) {
            //     g_pcm_buf = aos_malloc_check(3200);
            // }
            char * bytes = data;
            uint16_t * mic1 = (uint16_t * )  bytes;
            uint16_t * mic2 = (uint16_t * ) &bytes[len/5];
            uint16_t * ref1 = (uint16_t * ) &bytes[len/5 * 2];
            uint16_t * ref2 = (uint16_t * ) &bytes[len/5 * 3];
            uint16_t * ace  = (uint16_t * ) &bytes[len/5 * 4];

            
            for (int i = 0; i < len / 5 / 2; i++) {
                pcm_buf[i * 5 + 0] = mic1[i];
                pcm_buf[i * 5 + 1] = mic2[i];
                pcm_buf[i * 5 + 2] = ref1[i];
                pcm_buf[i * 5 + 3] = ref2[i];
                pcm_buf[i * 5 + 4] = ace[i];
            }

            mic_copy_data((uint8_t *)pcm_buf, len);

            break;

        default:
            break;
    }
}

static int control_source_pcm(int start)
{
    yv_t * yv_handle = (yv_t *)mic_get_privdata();
    char *filepath = TMP_RECORD_FILE;

    if (start) {
        LOGD(TAG, "mic rec start.");
        filefd = aos_open(filepath, O_WRONLY | O_CREAT | O_TRUNC);
        CHECK_RET_WITH_RET(filefd >= 0, -1);

        yv_debug_hook(mic_event_cb, 1);
        yv_pcm_enable(yv_handle, 15);
        // g_mic_record = 1;
    } else {
        LOGD(TAG, "mic rec stop");
        aos_close(filefd);
        filefd = -1;

        yv_debug_hook(NULL, 0);
        yv_pcm_enable(yv_handle, 0);
        // g_mic_record = 0;
    }

    return 0;
}

static void media_state_cb(uint32_t event_id, const void *param, void *context)
{
    switch (event_id) {
        case EVENT_MEDIA_MUSIC_ERROR:
            aos_event_set(&evt_media_stat, EVT_MEDIA_ERROR, AOS_EVENT_OR);
            break;

        case EVENT_MEDIA_MUSIC_FINISH:
            aos_event_set(&evt_media_stat, EVT_MEDIA_FINISHED, AOS_EVENT_OR);
            break;
        default:;
    }
}

/**
 * AT+VOICE=4,white_noise.wav,192.168.1.104,8090
 */
static int voice_test(int isquery, int arg_num, char **args, char *test_result)
{
    #define SD_PATH "file:///fatfs0/%s"

    int ret;

    if (!isquery && arg_num == 4) {
        int mode = atoi(args[0]);
        char *wavfile = args[1];
        char *ip = args[2];
        char *port = args[3];
        char wavpath[128];
        uint32_t record_size;
        int read_bytes, sent_bytes;
        int sent_cnt = 0;

        if (mode < 1 || mode > 4) {
            return FTEST_RESULT_ERR_PARAM;
        }

        for (int i = 0; i < 50 && !wifi_internet_is_connected(); ++i) {
            aos_msleep(100);
        }

        if (!wifi_internet_is_connected()) {
            strcpy(test_result, "network err");
            return FTEST_RESULT_ERR_FAILED;
        }

        snprintf(wavpath, 128, SD_PATH, wavfile);
        LOGD(TAG, "play sin wave");

        aui_player_stop(MEDIA_SYSTEM);
        aos_event_set(&evt_media_stat, 0, AOS_EVENT_AND);        
        aui_player_play(MEDIA_MUSIC, wavpath, 0);

        ret = control_source_pcm(1);
        CHECK_RET_WITH_RET(ret == 0, FTEST_RESULT_ERR_FAILED);

        unsigned int flags;
        aos_event_get(&evt_media_stat, EVT_MEDIA_FINISHED | EVT_MEDIA_ERROR, AOS_EVENT_OR_CLEAR, &flags, AOS_WAIT_FOREVER);
        control_source_pcm(0);
        if (flags & EVT_MEDIA_ERROR) {
            return FTEST_RESULT_ERR_FAILED;
        }

        LOGD(TAG, "send to %s:%s", ip, port);

        ret = pcm_send_start(ip, port, &record_size);
        if (ret != 0 || record_size < CHUNK_SIZE) {
            pcm_send_stop();
            return FTEST_RESULT_ERR_FAILED;
        }

        while (sent_cnt++ < 1000) {
            record_get_count(rec_hdl, &read_bytes, &sent_bytes);
            if (record_size - CHUNK_SIZE < (uint32_t)read_bytes) {
                aos_msleep(100);
                LOGD(TAG, "file sent");
                break;
            }

            aos_msleep(30);
        }

        ret = pcm_send_stop();
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

    aos_event_new(&evt_media_stat, 0);
    event_subscribe(EVENT_MEDIA_MUSIC_FINISH, media_state_cb, NULL);
    event_subscribe(EVENT_MEDIA_MUSIC_ERROR, media_state_cb, NULL);

    task = utask_new("fct", 8 * 1024, 150, AOS_DEFAULT_APP_PRI + 5);
    CHECK_RET_WITH_GOTO(task, ERR);

    fct_init(1, 115200, task);

    fct_register_cmd("FCTSTAT",     NULL, test_fctstate);
    fct_register_cmd("FCTPASSED",   NULL, test_passed);
    fct_register_cmd("GPIO",        NULL, gpio_test);
    fct_register_cmd("MAC",         NULL, mac_test);
    fct_register_cmd("ALIDEVINFO",  NULL, alidevinfo_test);
    fct_register_cmd("RAM",         NULL, ram_test);
    fct_register_cmd("RTC",         NULL, rtc_test);
    fct_register_cmd("FLASH",       NULL, flash_test);
    fct_register_cmd("SDCARD",      NULL, sdcard_test);
    fct_register_cmd("RFCAL",       NULL, rfcal_test);
    fct_register_cmd("ADCCAL",      NULL, adccal_test);
    fct_register_cmd("KEY",         NULL, key_test);
    fct_register_cmd("LED",         NULL, led_test);
    fct_register_cmd("SLEEP",       NULL, low_power_test);
    fct_register_cmd("VOICE",       NULL, voice_test);

    while (1) aos_msleep(10000);
    
    return;

ERR:
    LOGD(TAG, "fct init failed");
}
