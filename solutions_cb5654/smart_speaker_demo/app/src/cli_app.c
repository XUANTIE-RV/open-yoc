/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include "app_main.h"
#include "app_lpm.h"
#include <http_client.h>
#include <yoc/record.h>

#define CMD_INFO "usage:\n"LPM_ENTER""WIFI_PROV""ALARM_CMD

#ifdef AW2013_ENABLED
#define AD2014_LED "    aw2013 test: app led_rgb <led_id> <mode> <dime>\n"
#endif
#ifdef AW9523_ENABLED
#define AW9523_LED "    aw9523 test: app led_rgb <led_id> <mode> <dime>\n"
#endif
#define LPM_ENTER "    switch lpm mode: app lpm 1|2\n"
#define WIFI_PROV "    switch wifi prov: app wifi_prov softap/dev_ap/smartconfig\n"

#define ALARM_CMD "    edit clock: app clock 2 clock_id period hh mm ss\n      period:0-once,1-day,2-week,3-workday\n      clock_id:0-add 1~5-edit\n    del clock: app clock 1 clock_id\n"

#define TEST_TIMES 1
#define TEST_BUFFER_SIZE (1*1024*1024)
#define TEST_DATA_MBYTE (TEST_TIMES*TEST_BUFFER_SIZE/1024/1024)

static void lfs_test_read(void)
{
    int fd = aos_open("/misc/lfs_test.txt", O_RDONLY);
    if (fd < 0) {
        printf("file not create, pls create first\n");
    }

    int   buff_size = TEST_BUFFER_SIZE;
    char *buff      = malloc(buff_size);
    memset(buff, 0xAA, buff_size);

    printf("START\n");
    int beg = aos_now_ms();
    for (int i = 0; i < TEST_TIMES; i++) {
        if (aos_read(fd, buff, buff_size) < 0) {
            printf("read error\n");
        }
    }
    int end = aos_now_ms();
    printf("END");
    printf("%.3fMB/s",  (TEST_DATA_MBYTE * 1000.0) / (end - beg) );

    free(buff);
    aos_close(fd);
}

static void lfs_test_write(void)
{
    int i = 0;
    int total = 0;
    int   buff_size = TEST_BUFFER_SIZE;
    char *buff      = malloc(buff_size);
    memset(buff, 0x55, buff_size);

    for (i = 0; i < TEST_TIMES; i++) {
        int fd = aos_open("/misc/lfs_test.txt", O_CREAT | O_RDWR);
        if (fd < 0) {
            printf("aos_open fail");
        }

        printf("START");
        int beg = aos_now_ms();
            if (aos_write(fd, buff, buff_size) < 0) {
                printf("write error\n");
            }
        int end = aos_now_ms();
        printf("END");
        total += (end - beg);
        aos_close(fd);
    }
    printf("%.3fMB/s",  (TEST_DATA_MBYTE * 1000.0) / total);
    free(buff);
}

void http_download(const char *url, const char *save_dir)
{
    int http_response_size = 4 * 1024;
    char *buffer = aos_malloc_check(http_response_size + 1);
    int rec_fd = 0;
    char save_path[128];

    if (!save_dir || strlen(save_dir) < 2) {
        printf("save path err\n");
        return;
    }

    if (!wifi_internet_is_connected()) {
        printf("network not connected!\n");
        return;
    }

    if (save_dir[strlen(save_dir) - 1] == '/') {
        snprintf(save_path, 128, "%s%s", save_dir, strrchr(url, '/') + 1);
    } else {
        snprintf(save_path, 128, "%s/%s", save_dir, strrchr(url, '/') + 1);
    }

    rec_fd = aos_open(save_path, O_WRONLY | O_CREAT | O_TRUNC);
    if (rec_fd < 0) {
        printf("Create file %s error\n", save_path);
        return;
    }

    http_client_config_t config = {
        .url = url,
        //.event_handler = _http_event_handler,
        .timeout_ms = 10000,
        .buffer_size = 4 * 1024,
        // .cert_pem = ca_cert,
    };

    http_client_handle_t client = http_client_init(&config);
    if (client == NULL) {
        printf("http request init failed, url \"%s\"\n", url);
        return;
    }

    http_errors_t err;
    if ((err = http_client_open(client, 0)) != HTTP_CLI_OK) {
        printf("Failed to open HTTP connection: 0x%x\n", err);
        goto http_err;
    }

    int content_length =  http_client_fetch_headers(client);
    int total_read_len = 0, read_len;
    while (total_read_len < content_length) {
        read_len = http_client_read(client, buffer, http_response_size);
        if (read_len <= 0) {
            printf("Error read data\n");
            goto http_err;
        }

        int wsize = aos_write(rec_fd, buffer, read_len);
        if (wsize != read_len) {
            printf("write sd failed %d %d\n", wsize, read_len);
            goto http_err;            
        }

        total_read_len += read_len;
    }
    printf("download finished, save to %s!\n", save_path);

http_err:    
    http_client_cleanup(client);
    aos_free(buffer);
    aos_close(rec_fd);
}

#define CHUNK_SIZE (5 * 3200)
static int ws_file_send(const char *ip, const char *port, const char *file_path)
{
    char buf[64];
    char   buf2[128];
    struct stat fstat;
    char ws_uri[128];
    uint32_t total_size;
    int ret, success = 0;
    rec_hdl_t rec_hdl = NULL;

    ret = aos_stat(file_path, &fstat);
    if (ret < 0) {
        printf("access file %s error\n", file_path);
        return -1;
    }

    total_size = fstat.st_size;

    snprintf(ws_uri, 128, "ws://%s:%s", ip, port);
    snprintf(buf, sizeof(buf), "sdbuffer://handle=%s", file_path);
    snprintf(buf2, sizeof(buf2), "%s/%s", ws_uri, strrchr(file_path, '/') + 1);
    
    rec_hdl = record_register(buf, buf2);
    if (!rec_hdl) {
        printf("ws register failed\n");
        goto END;
    }

    // record_set_data_ready_cb(rec_hdl, data_ready, NULL);
    record_set_chunk_size(rec_hdl, CHUNK_SIZE);
    ret = record_start(rec_hdl);
    if (ret == 0) {
        int read_bytes, sent_bytes;
        int last_send_bytes = 0;
        int stuck_cnt = 0;

        while (1) {
            record_get_count(rec_hdl, &read_bytes, &sent_bytes);
            if (total_size - CHUNK_SIZE < (uint32_t)read_bytes) {
                aos_msleep(500);
                success = 1;
                break;
            }

            if (last_send_bytes == sent_bytes) {
                if (++stuck_cnt > 50) {
                    printf("send timeout!\n");
                    break;
                }
            } else {
                stuck_cnt = 0;
            }

            last_send_bytes = sent_bytes;
            aos_msleep(100);
        }

        ret = record_stop(rec_hdl);
    }

END:
    record_unregister(rec_hdl);
    
    if (success) {
        printf("file %s sent success\n", file_path);
    }
    return 0;
}

static void cmd_app_func(char *wbuf, int wbuf_len, int argc, char **argv)
{
    if (argc < 3) {
        printf(CMD_INFO);
        return;
    }

    if (strcmp(argv[1], "led") == 0) {

        if (strcmp(argv[2], "off") == 0) {
            app_set_led_enabled(0);
            app_set_led_state(LED_TURN_OFF);
        } else {
            app_set_led_enabled(1);

            int stat = atoi(argv[2]);
            app_set_led_state(stat);
        }
    }
#ifdef AW2013_ENABLED
    else if (strcmp(argv[1], "led_rgb") == 0) {
        if (argc != 5) {
            printf("app led_rgb <led_id> <mode> <dime>");
        }

        aw2013_led_control(&g_i2c_dev, atoi(argv[2]), atoi(argv[3]), atoi(argv[4]));
    }
#endif
#ifdef AW9523_ENABLED
	else if (strcmp(argv[1], "led_array") == 0) {
        if (argc != 4) {
            printf("app led_array <led_id> <dime>");
        }

        if (strcmp(argv[2], "all") == 0) {
            for (int i = LED_ARRAY_1; i <= LED_ARRAY_12; ++i) {
                aw9523_led_control(&g_i2c_dev, i, atoi(argv[3]));
            }
        } else {
            aw9523_led_control(&g_i2c_dev, atoi(argv[2]), atoi(argv[3]));
        }
    } else if (strcmp(argv[1], "aw9523_input") == 0) {
        uint16_t val;
        aw9523_read_input(&g_i2c_dev, &val, atoi(argv[2]));
        printf("aw9523 input: %x\n", val);
    } 
#endif
    else if (strcmp(argv[1], "lpm") == 0) {
        int policy = atoi(argv[2]);
        if (policy) {
            if(policy == LPM_POLICY_LOW_POWER ||
               policy == LPM_POLICY_DEEP_SLEEP) {
                app_lpm_sleep(policy, 0);
            } else {
                printf("lpm policy (%d) set error\n", atoi(argv[2]));
                return;
            }
        }
    } else if (strcmp(argv[1], "power_off") == 0) {
        app_lpm_sleep(LPM_POLICY_DEEP_SLEEP, 1);
    } else if (strcmp(argv[1], "wifips") == 0) {
        wifi_set_lpm(atoi(argv[2]));
    } else if (strcmp(argv[1], "voldbg") == 0) {
        //extern void snd_card_register(int vol_range, int fix_l_gain, int fix_r_gain);
        //snd_card_register(0, 0, 0);
        //aui_player_vol_set(MEDIA_MUSIC, 100);
    } else if (strcmp(argv[1], "mac") == 0) {
        uint8_t mac[6];
        wifi_getmac(mac);
        printf("%02x:%02x:%02x:%02x:%02x:%02x\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    } else if (strcmp(argv[1], "mic") == 0) {
        if (argc == 4) {
extern int set_adc2_left_channel_mic_analog_gain(int gain);
extern int set_adc2_right_channel_mic_analog_gain(int gain);
            set_adc2_left_channel_mic_analog_gain(atoi(argv[3]));
            set_adc2_right_channel_mic_analog_gain(atoi(argv[3]));
            printf("mic gain(0-31 8=0dB step 1.5dB) %d\n", atoi(argv[3]));
int set_adc2_left_channel_micboost_gain(int gain);
int set_adc2_right_channel_micboost_gain(int gain);
            set_adc2_left_channel_micboost_gain(atoi(argv[2]));
            set_adc2_right_channel_micboost_gain(atoi(argv[2]));
            printf("mic boost(0=0dB 1=6dB 2=12dB 3=20dB) %d\n", atoi(argv[2]));
        }
    } else if (strcmp(argv[1], "ref") == 0) {
extern int set_adc1_left_channel_other_analog_gain(int gain);
extern int set_adc1_right_channel_other_analog_gain(int gain);
        if (argc == 3) {
            printf("ref gain(0-31 16=0dB step 0.75dB) %d\n", atoi(argv[2]));
            set_adc1_left_channel_other_analog_gain(atoi(argv[2]));
            set_adc1_right_channel_other_analog_gain(atoi(argv[2]));
        }
    } else if (strcmp(argv[1], "pa") == 0) {
        int en = atoi(argv[2]);
        app_audio_pa_ctrl(en);
        printf("set pa %d\n", en);
    } else if (strcmp(argv[1], "ldo") == 0) {
        if (argc == 3) {
            int ldo = atoi(argv[2]);

            extern void silan_dsp_ldo_disable();
            extern void silan_dsp_ldo_config(int level);
            extern void silan_soc_ldo_config(int level);
            switch(ldo)
            {
            case 0:
                printf("ldo disabled\n");
                silan_dsp_ldo_disable();
                break;
            case 1:
                printf("ldo 1.2v\n");
                silan_dsp_ldo_config(0 /* LDO_OUT_12V */);
                silan_soc_ldo_config(0);
                break;
            case 2:
                printf("ldo 1.0v\n");
                silan_dsp_ldo_config(1 /* LDO_OUT_10V */);
                silan_soc_ldo_config(1);
                break;
            case 3:
                printf("ldo 0.8v\n");
                silan_dsp_ldo_config(2 /* LDO_OUT_08V */);
                silan_soc_ldo_config(2);
                break;
            default:
                ;
            }
        }
    } else if (strcmp(argv[1], "battery") == 0) {
#ifdef CONFIG_BATTERY_ADC
        printf("battery: %d\n", app_battery_read());
#endif
    } else if (strcmp(argv[1], "rtc") == 0) {
#if CONFIG_CLOCK_ALARM
        if (argc <= 6) {
            int arg2 = atoi(argv[2]);
            switch(arg2)
            {
            case 1:
                rtc_from_system();
                break;
            case 2:
                rtc_to_system();
                break;
            case 3:
                rtc_to_system();
                rtc_debug();
                break;
            case 4:
            {
                int32_t week = atoi(argv[3]);
                int32_t hour = atoi(argv[4]);
                int32_t min = atoi(argv[5]);
                rtc_set_alarm(week, hour, min);
                break;
            }
            default:
                ;
            }
        }
#endif
    } else if (strcmp(argv[1], "clock") == 0) {
        int arg2 = atoi(argv[2]);
        int id = atoi(argv[3]);
        switch (arg2)
        {
            case 1:
                clock_alarm_set(id, NULL);
                printf("clock alarm: clock id %d is delete.\n", id);
                break;
            case 2:
            {
                clock_alarm_config_t cli_time;
                cli_time.period = atoi(argv[4]);
                cli_time.hour = atoi(argv[5]);
                cli_time.min = atoi(argv[6]);
                cli_time.sec = atoi(argv[7]);

                id = clock_alarm_set(id, &cli_time);
                if (id == -2) {
                    printf("clock alarm has repeat time.\n");
                    return;
                } else if (id == -1) {
                    printf("clock alarm has full.\n");
                    return;
                } if (id == -4) {
                    printf("clock alarm time config error.\n");
                    return;
                } else {
                    clock_alarm_enable(id, 1);
                    printf("clock alarm: clock id %d set success. time %d:%d:%d, period %d.\n",
                        id, cli_time.hour, cli_time.min, cli_time.sec, cli_time.period);
                }
                break;
            }
            default:
                ;
        }
    } else if (strcmp(argv[1], "wifi_prov") == 0) {
        extern int wifi_prov_method;
        if (strcmp(argv[2], "softap") == 0) {
            wifi_prov_method = WIFI_PROVISION_SOFTAP;
            printf("wifi provision method switch to softap!\n");
        } else if (strcmp(argv[2], "dev_ap") == 0) {
            wifi_prov_method = WIFI_PROVISION_SL_DEV_AP;
            printf("wifi provision method switch to smartliving device ap!\n");
        } else if (strcmp(argv[2], "smartconfig") == 0) {
            wifi_prov_method = WIFI_PROVISION_SL_SMARTCONFIG;
            printf("wifi provision method switch to smartliving smartconfig!\n");
        } else {
            printf("app wifi_prov softap/dev_ap/smartconfig");
        }
    } else if (strcmp(argv[1], "http_download") == 0) {
        if (argc == 4) {
            http_download(argv[3], argv[2]);
        } else {
            printf("app http_download <save directory> <url>\n");
        }
    } else if (strcmp(argv[1], "ws_file_send") == 0) {
        if (argc == 5) {
            ws_file_send(argv[2], argv[3], argv[4]);
        } else {
            printf("app ws_file_send <ip> <port> <file path>\n");
        }
    } else if (strcmp(argv[1], "lfs") == 0) {
        if (strcmp(argv[2], "read") == 0) {
            lfs_test_read();
        } else if (strcmp(argv[2], "write") == 0) {
            lfs_test_write();
        } else {
            printf("app lfs read/write");
        }
    }
    else {
        printf("app led/lpm/pa/mic/ref\n");
    }
}

void cli_reg_cmd_app(void)
{
    /* WiFi 测试CLI命令 */
    extern void iwpriv(char *wbuf, int wbuf_len, int argc, char **argv);
    static const struct cli_command cmd_info_wifi = {
        "iwpriv",
        "test iwpriv",
        iwpriv
    };
    aos_cli_register_command(&cmd_info_wifi);

    /* 其他CLI命令 */
    static const struct cli_command cmd_info_app = {"app", "app test cmd", cmd_app_func};
    aos_cli_register_command(&cmd_info_app);
}
