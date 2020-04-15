#include <devices/uart.h>
#include <yoc/atserver.h>
#include <aos/log.h>
#include <at_cmd.h>
#include <yoc/eventid.h>
#include <yoc/iot.h>
#include <yoc/netmgr.h>
#include <pin.h>
#include <devices/device.h>
#include <devices/led.h>
#include <devices/gprs.h>
#include <devices/gnss.h>
#include <devices/battery.h>
#include <yoc/atserver.h>
#include <drv/gpio.h>
#include <yoc/lpm.h>
#include <devices/devicelist.h>
#include <at_internal.h>

#include "app_main.h"

#define TAG "at_server_init"

#define S(x) #x
#define STR(x) S(x)

#define LED_ON  1
#define LED_OFF 0
#define DEVICE_LED_NAME     "ledrgb"
static led_pin_config_t led_config = {EXAMPLE_LED_R_PIN, EXAMPLE_LED_G_PIN, EXAMPLE_LED_B_PIN, 1};
extern const atserver_cmd_t at_cmd[];
extern int32_t at_cmd_event_on(void);

static iot_t         *iot_handle  = NULL;
static iot_channel_t *iot_ch_post = NULL;
static int  g_iot_connected = 0;
extern netmgr_hdl_t app_netmgr_hdl;
static aos_dev_t *g_gnss_dev;

#if 0
static void at_net_event_hdl(uint32_t event_id, const void *data, void *context)
{
    if (at_cmd_event_on()) {
        atserver_lock();

        if (event_id == EVENT_NETMGR_GOT_IP) {
            atserver_send("+EVENT=%s,%s\r\n", "NET", "LINK_UP");
        } else if (event_id == EVENT_NETMGR_NET_DISCON) {
            atserver_send("+EVENT=%s,%s\r\n", "NET", "LINK_DOWN");
        }

        atserver_unlock();
    }

    return;
}
#endif

static void get_gnss_info()
{
    gnss_info_t info;
    int ret = gnss_getinfo(g_gnss_dev, &info);

    if (ret == 0) {
        LOGD(TAG, "\n\tgnss info:"
             "\n\tstar_num            : %d"
             "\n\tpositon_indicator   : %c"
             "\n\tlongitude           : %lf"
             "\n\tlongitude_indicator : %c"
             "\n\tlatitude            : %lf"
             "\n\tlatitude_indicator  : %c"
             "\n\taltitude            : %lf"
             "\n\tspeed               : %lf"
             "\n\tutc_time            : %lf"
             "\n\tdata                : %d"
             "\n\thdop                : %lf",
             info.star_num, info.positon_indicator, info.longitude, info.longitude_indicator,
             info.latitude, info.latitude_indocator,
             info.altitude, info.speed, info.time_utc, info.data, info.hdop);

    }
}

static void set_gpio(int value)
{
    static gpio_pin_handle_t pin_hd = NULL;

    if (pin_hd == NULL) {
        pin_hd = csi_gpio_pin_initialize(PA4, NULL);
        csi_gpio_pin_config(pin_hd, GPIO_MODE_PUSH_PULL, GPIO_DIRECTION_OUTPUT);
    }
    csi_gpio_pin_write(pin_hd, value);
}

static void led_set_status(int status)
{
    static aos_dev_t *led_dev = NULL;

    if (led_dev == NULL) {
        led_dev = led_open_id(DEVICE_LED_NAME, 1);
    }

    if (led_dev == NULL) {
        LOGE(TAG, "led_dev open err!");
        return;
    }

    if (status == LED_ON) {
        led_control(led_dev, COLOR_WHITE, -1, -1);
    } else if (status == LED_OFF) {
        led_control(led_dev, COLOR_BLACK, -1, -1);
    }

    //led_close(led_dev);
}

#if 0
void at_net_event_sub_init(void)
{
    event_subscribe(EVENT_NETMGR_GOT_IP, at_net_event_hdl, NULL);
    event_subscribe(EVENT_NETMGR_NET_DISCON, at_net_event_hdl, NULL);
}

void at_net_event_unsub_init(void)
{
    event_unsubscribe(EVENT_NETMGR_GOT_IP, at_net_event_hdl, NULL);
    event_unsubscribe(EVENT_NETMGR_NET_DISCON, at_net_event_hdl, NULL);
}
#endif

static void channel_set(uData *udata, void *arg)
{
    uData *node;
    slist_for_each_entry(&udata->head, node, uData, head) {
        if (node->value.type == TYPE_STR) {
            LOGI(TAG, "ch set s (%s,%s)", node->key.v_str, node->value.v_str);

            if (strcmp(node->key.v_str, "downlink") == 0) {
                if (at_cmd_event_on()) {
                    atserver_lock();
                    atserver_send("+RECV=%d,%s\r\n", strlen(node->value.v_str), node->value.v_str);
                    atserver_unlock();
                }
            }
        }
    }
}

static void channel_get(uData *udata, void *arg)
{
    // int temp = 0, humi = 0;

    // //LOGI(TAG, "channel_get\r\n");
    // yoc_udata_set(udata, value_s("temp"), value_i(temp), 1);
    // yoc_udata_set(udata, value_s("humi"), value_i(humi), 1);
    // yoc_udata_set(udata, value_s("led"), value_i(g_led_status), 1);
    // yoc_udata_set(udata, value_s("deviceName"), value_s(aos_get_device_name()), 1);
}

static void init_channel_udata(uData *udata)
{
    //LOGI(TAG, "init_channel_udata\r\n");
    //yoc_udata_set(udata, value_s("uplink"), value_s("test"), 1);
    yoc_udata_set(udata, value_s("downlink"), value_s("test"), 0);
    // yoc_udata_set(udata, value_s("led"), value_i(g_led_status), 1);
    yoc_udata_set(udata, value_s("deviceName"), value_s(aos_get_device_name()), 0);
}

static int channel_open(void)
{
    /* one channel include two topic(publish&subscribe)
    publish data to topic /[product_key]/[device_name]/update
    subscribe topic /[product_key]/[device_name]/get */
    iot_ch_post = iot_channel_open(iot_handle, "thing/event/property/post,thing/service/property/set");

    if (iot_ch_post == NULL) {
        LOGE(TAG, "iot ch open");
        return -1;
    }

    iot_channel_config(iot_ch_post, channel_set, channel_get, NULL);

    /* init data node */
    init_channel_udata(iot_ch_post->uData);

    iot_channel_start(iot_ch_post);

    return 0;
}

void channel_close(void)
{
    if (iot_ch_post) {
        iot_channel_close(iot_ch_post);
        iot_ch_post = NULL;
    }
}

static void iot_event(uint32_t event_id, const void *param, void *context)
{
    if (iot_ch_post == NULL) {
        return;
    }

    switch (event_id) {
        case EVENT_IOT_CONNECT_SUCCESS:
            LOGD(TAG, "CONNNECT SUCCESS");

            if (at_cmd_event_on()) {
                atserver_lock();
                atserver_send("+EVENT=%s,%s\r\n", "IOT", "READY");
                atserver_unlock();
            }

            g_iot_connected = 1;
            break;

        case EVENT_IOT_CONNECT_FAILED:
        case EVENT_IOT_DISCONNECTED:
            g_iot_connected = 0;

            if (at_cmd_event_on()) {
                atserver_lock();
                atserver_send("+EVENT=%s,%s\r\n", "IOT", "DOWN");
                atserver_unlock();
            }

            channel_close();
            break;

        case EVENT_IOT_PUSH_SUCCESS:
            LOGD(TAG, "PUSH_SUCCESS");

            if (at_cmd_event_on()) {
                atserver_lock();
                atserver_send("+EVENT=%s,%s\r\n", "IOT", "SEND OK");
                atserver_unlock();
            }

            break;

        case EVENT_IOT_PUSH_FAILED:

            if (at_cmd_event_on()) {
                atserver_lock();
                atserver_send("+EVENT=%s,%s\r\n", "IOT", "SEND FAILED");
                atserver_unlock();
            }

            break;

        default:
            ;
    }

    /*do exception process */
    app_exception_event(event_id);
}

void sms_cb(char *oa, char *content, int length)
{
    LOGD(TAG, "RECV SMS FROM: %s, content: %s", oa, content);
}

static void network_event(uint32_t event_id, const void *param, void *context)
{
    switch (event_id) {
        case EVENT_NETMGR_GOT_IP:
            LOGD(TAG, "EVENT_NETMGR_GOT_IP");

            if (at_cmd_event_on()) {
                atserver_lock();
                atserver_send("+EVENT=%s,%s\r\n", "NET", "LINK_UP");
                atserver_unlock();
            }

            if (g_gnss_dev) {
                gnss_close(g_gnss_dev);
            }

            g_gnss_dev = gnss_open("gnss");

            /* try again */
            if (g_gnss_dev == NULL) {
                g_gnss_dev = gnss_open("gnss");
            }

            /* try again */
            if (g_gnss_dev == NULL) {
                g_gnss_dev = gnss_open("gnss");
            }

            if (g_gnss_dev == NULL) {
                LOGE(TAG, "GNSS OPEN failed");
                aos_check_param(NULL);
            }

            aos_dev_t *dev;
            dev = netmgr_get_dev(app_netmgr_hdl);

            hal_gprs_sms_set_cb(dev, sms_cb);

            // int ret = hal_gprs_sms_send(dev, NULL, "1371234xxxx", "hello");

            // if (ret != 0) {
            //     LOGE(TAG, "SMS SEND FAILED");
            // }

            char imei[20], imsi[20];

            hal_gprs_get_imei(dev, imei);
            hal_gprs_get_imsi(dev, imsi);

            LOGD(TAG, "IMEI: %s", imei);
            LOGD(TAG, "IMSI: %s", imsi);

            int iot_en = 1;

            aos_kv_getint("iot_en", &iot_en);

            if (iot_en == 1) {
                if (g_iot_connected == 0) {
                    channel_open();
                }
            }

            break;

        case EVENT_NETMGR_NET_DISCON:
            if (at_cmd_event_on()) {
                atserver_lock();
                atserver_send("+EVENT=%s,%s\r\n", "NET", "LINK_DOWN");
                atserver_unlock();
            }

            channel_close();
            g_iot_connected = 0;
            break;
    }

    /*do exception process */
    app_exception_event(event_id);
}

static void user_local_event_cb(uint32_t event_id, const void *param, void *context)
{
    if (event_id == EVENT_CHANNEL_CHECK) {
        if (g_iot_connected == 0) {
            int res = -1;

            if (netmgr_is_gotip(app_netmgr_hdl)) {
                res = channel_open();
            } else {
                LOGW(TAG, "net not ready");
            }

            /* error before do connnect, trans to EVENT_IOT_CONNECT_FAILED to retry */
            if (res != 0) {
                event_id = EVENT_IOT_CONNECT_FAILED;
            }
        } else {
            LOGD(TAG, "channel already open");
        }

    } else if (event_id == EVENT_CHANNEL_CLOSE) {

        if (at_cmd_event_on()) {
            atserver_lock();
            atserver_send("+EVENT=%s,%s\r\n", "IOT", "DOWN");
            atserver_unlock();
        }
        channel_close();
        g_iot_connected = 0;
        event_id = EVENT_IOT_CONNECT_FAILED;
    } else if (event_id == EVENT_GNSS_GET) {
        get_gnss_info();
        //event_publish_delay(EVENT_GNSS_GET, NULL, 10000);
    }

    /*do exception process */
    app_exception_event(event_id);
}

/* AT+SEND? */
/* AT+SEND=<data_len>,<data> */
void at_cmd_app_send(char *cmd, int type, char *data)
{
    switch (type) {
        case WRITE_CMD :
            if (iot_ch_post) {
                int len;

                char *str = calloc(strlen(data), 1);

                if (str == NULL) {
                    AT_BACK_ERR();
                    return;
                }

                int ret = sscanf(data, "%d,%s", &len, str);

                if (ret < 0 || ret != 2) {
                    free(str);
                    LOGE(TAG, "parse");
                    AT_BACK_ERR();
                    return;
                }

                if (len != strlen(str)) {
                    AT_BACK_ERR();
                    free(str);
                    return;
                }

                yoc_udata_set(iot_ch_post->uData, value_s("uplink"), value_s(str), 1);
                iot_channel_push(iot_ch_post, 0);
                free(str);
                AT_BACK_OK();
                return;
            }

            AT_BACK_ERR();
            break;

        default :
            AT_BACK_ERR();
            break;
    }
}

/* AT+GET? */
void at_cmd_app_get(char *cmd, int type, char *data)
{
    char buf[120];
    switch (type) {
        case WRITE_CMD :
        if (strcmp(data, "gnss") == 0) {
            if (g_gnss_dev) {
                gnss_info_t info;

                int ret = gnss_getinfo(g_gnss_dev, &info);

                if (ret == 0) {
                    LOGD(TAG, "\n\tgnss info:"
                         "\n\tstar_num            : %d"
                         "\n\tpositon_indicator   : %c"
                         "\n\tlongitude           : %lf"
                         "\n\tlongitude_indicator : %c"
                         "\n\tlatitude            : %lf"
                         "\n\tlatitude_indicator  : %c"
                         "\n\taltitude            : %lf"
                         "\n\tspeed               : %lf"
                         "\n\tutc_time            : %lf"
                         "\n\tdata                : %d"
                         "\n\thdop                : %lf",
                         info.star_num, info.positon_indicator, info.longitude, info.longitude_indicator,
                         info.latitude, info.latitude_indocator,
                         info.altitude, info.speed, info.time_utc, info.data, info.hdop);

                    snprintf(buf, sizeof(buf), "%c,%.6lf,%c,%.6lf,%c,%.6lf,%.2lf,%d,%d,%.2lf,%6f",
                         info.positon_indicator,
                         info.longitude, info.longitude_indicator,
                         info.latitude, info.latitude_indocator, info.altitude, info.speed, (uint32_t)info.time_utc, info.data, info.hdop);



                    AT_BACK_RET_OK(cmd, buf);

                } else {
                    AT_BACK_ERR();
                }


                return;
            }
        } else if (strcmp(data, "gprs") == 0) {
            char iccid[21];
            int is_insert;
            int csq;
            aos_dev_t *netif_dev;
            int ret;
            netif_dev  = netmgr_get_dev(app_netmgr_hdl);

            if (netif_dev == NULL) {
                goto error;
            }

            ret = hal_gprs_get_simcard_info(netif_dev, iccid, &is_insert);

            if (ret) {
                goto error;
            }

            ret = hal_gprs_get_csq(netif_dev, &csq);

            if (ret) {
                goto error;
            }

            snprintf(buf, sizeof(buf), "%s,%d",iccid, csq);

            printf("hal_gprs_get_simcard_info: %s, %d\n", iccid, csq);

            AT_BACK_RET_OK(cmd, buf);

            return; 
        } else if (strcmp(data, "adc") == 0) {
            static aos_dev_t *battry = NULL;
            battery_voltage_t battery_val;
            if (battry == NULL) {
                battry = battery_open_id("battery_fetch", 1);
            }

            if (battery_fetch(battry, VOLTAGE) == 0) {
                if (battery_getvalue(battry, VOLTAGE, &battery_val, sizeof(battery_voltage_t)) == 0) {
                    LOGD(TAG, "read battery: %d mv", battery_val.volt * 37);
                    snprintf(buf, sizeof(buf), "%d", battery_val.volt * 37);
                    AT_BACK_RET_OK(cmd, buf);

                    return;
                } else {
                    LOGD(TAG, "battery get failed");
                    goto error;
                }
            }

        }

error:
            AT_BACK_ERR();
            break;

        default :
            AT_BACK_ERR();
            break;
    }
}

void at_cmd_app_set(char *cmd, int type, char *data)
{
    switch (type) {
        case WRITE_CMD : {

            if (strncmp(data, "gpio", 4) == 0) {
                if (data[5] == '0') {
                    set_gpio(0);
                    AT_BACK_OK();
                } else if (data[5] == '1') {
                    set_gpio(1); 
                    AT_BACK_OK();
                } else {
                    AT_BACK_ERR();
                }

                return;
            }
            AT_BACK_ERR();
            break;
        }
        default :
            AT_BACK_ERR();
            break;
    }
}

void at_cmd_app_sleep(char *cmd, int type, char *data)
{
    switch (type) {
        case EXECUTE_CMD : {
            AT_BACK_OK();
            pm_agree_halt(0);

            break;
        }
        case WRITE_CMD : {
            if (data[0] >= '0' && data[1] <= '9') {
                AT_BACK_OK();
                LOGD(TAG, "pm_agree_halt: %d", atoi(data));
                pm_agree_halt(atoi(data) * 1000);
            } else {
                AT_BACK_ERR();
            }
            break;
        }
        default :
            AT_BACK_ERR();
            break;
    }
}

void at_server_init(utask_t *task)
{
    uart_config_t config;
    uart_config_default(&config);

    if (task == NULL) {
        task = utask_new("at_srv", 2 * 1024, QUEUE_MSG_COUNT, AOS_DEFAULT_APP_PRI);
    }

    if (task) {
        atserver_init(task, "uart1", &config);
        atserver_set_output_terminator("");
        atserver_add_command(at_cmd);
    }

    //at_net_event_sub_init();

    /* init the led for system status notify */
    led_rgb_register(&led_config, 1);

    led_set_status(LED_ON);
    /* Subscribe */
    event_subscribe(EVENT_NETMGR_GOT_IP, network_event, NULL);
    event_subscribe(EVENT_NETMGR_NET_DISCON, network_event, NULL);

    event_subscribe(EVENT_DATA_REPORT, user_local_event_cb, NULL);
    event_subscribe(EVENT_GNSS_GET, user_local_event_cb, NULL);
    event_subscribe(EVENT_CHANNEL_CHECK, user_local_event_cb, NULL);
    event_subscribe(EVENT_CHANNEL_CLOSE, user_local_event_cb, NULL);

    /* Subscribe IOT Event */
    event_subscribe(EVENT_IOT_CONNECT_SUCCESS, iot_event, NULL);
    event_subscribe(EVENT_IOT_CONNECT_FAILED, iot_event, NULL);
    event_subscribe(EVENT_IOT_DISCONNECTED, iot_event, NULL);
    event_subscribe(EVENT_IOT_PUSH_SUCCESS, iot_event, NULL);
    event_subscribe(EVENT_IOT_PUSH_FAILED, iot_event, NULL);

    event_publish_delay(EVENT_GNSS_GET, NULL, 20000);

    /* [product_key].iot-as-coap.cn-shanghai.aliyuncs.com:1883 */
    iot_alimqtt_config_t cfg = {.server_url_suffix =
                                    "iot-as-mqtt.cn-shanghai.aliyuncs.com:1883"
                               };

    iot_handle = iot_new_alimqtt(&cfg);
    //atserver_cmd_show();
}
