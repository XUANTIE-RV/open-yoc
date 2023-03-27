/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifdef CONFIG_BT

#include <yoc/atserver.h>
#include "../at_internal.h"
#include <aos/ble.h>
#include <aos/kv.h>
#include "at_ble.h"
#include "at_ble_def_config.h"
#include <yoc/partition.h>

#ifdef CONFIG_DUT_SERVICE_ENABLE
#include <dut/hal/common.h>
#endif

#define TAG "at_ble_cmd"

#define UART_BUF_SIZE 16
#define GET_TX_POWER_FAILED 0XFFFFFFFF

#define AT_BLE_UART_CONFIG_SLAVE(server) server.at_config.uart_conf.slave_conf
#define AT_BLE_UART_CONFIG_MASTER(server) server.at_config.uart_conf.master_conf

#define AD_DATA_LENGTH 31
#define DEV_NAME_MAX_LENGTH 21

typedef struct _temp_adv_ad {
    ad_data_t *ad;
    uint8_t ad_num;
    uint8_t ad_data[AD_DATA_LENGTH];
} temp_adv_ad;

typedef struct _at_ble {
    uint8_t init_flag;
    uint8_t at_mode;
    uint8_t uart_buffer[AT_BLE_UART_BUFFER_SIZE];

    uint8_t temp_role_set;
    adv_param_t temp_adv_set;
    uint8_t temp_ad_set[AD_DATA_LENGTH];
    uint8_t temp_sd_set[AD_DATA_LENGTH];
    at_conf_load at_config;
    at_ble_cb recv_cb;
} at_ble;

static at_ble ble_server;
static at_server_handler ble_server_handler;



typedef int partition_t;


typedef struct _ad_data {
    uint8_t type;
    uint8_t len;
    uint8_t data[31];
} ad_data;


extern uint8_t llState;

uint8_t length_temp = 0;
static ad_data_t *ad_out = NULL;
static ad_data_t *sd_out = NULL;




static tx_power tx_power_table[13] = {{-30, 0X00}, {-20, 0X01}, {-15, 0x02}, {-10, 0x03}, {-6, 0x04}, {-5, 0x05}, \
    {-3, 0x06}, {0, 0x0a}, {3, 0x0f}, {4, 0x12}, {5, 0x17}, {6, 0x1F}, {10, 0x3F}
};


_STATIC uint8_t char2u8(char c)
{
    if (c >= '0' && c <= '9') {
        return (c - '0');
    } else if (c >= 'a' && c <= 'f') {
        return (c - 'a' + 10);
    } else if (c >= 'A' && c <= 'F') {
        return (c - 'A' + 10);
    } else {
        return 0;
    }
}

_STATIC int str2hex(uint8_t hex[], char *s, uint8_t cnt)
{
    uint8_t i;

    if (!s) {
        return 0;
    }

    for (i = 0; (*s != '\0') && (i < cnt); i++, s += 2) {
        hex[i] = ((char2u8(*s) & 0x0f) << 4) | ((char2u8(*(s + 1))) & 0x0f);
    }

    return i;
}


void at_cmd_bt_name(char *cmd, int type, char *data)
{
    if (!ble_server.init_flag) {
        AT_BACK_ERRNO(ERR_AT(AT_ERR_AT_SERVICE_NOT_INITIALIZED));
        return;
    }

    int ret = 0;

    if (type == WRITE_CMD) {
        int len = strlen(data);

        if (len > 0 && len <= DEV_NAME_MAX_LENGTH) {
            ret = aos_kv_setstring("BT_NAME", data);

            if (!ret) {
                AT_BACK_OK();
            } else {
                AT_BACK_ERRNO(ERR_AT(AT_ERR_BT_KV_SAVE_PARAM));
            }
        } else {
            AT_BACK_ERRNO(ERR_AT(AT_ERR_INVAL));
        }
    } else if (type == READ_CMD) {
        AT_BACK_RET_OK_HEAD(cmd, ble_server.at_config.bt_name);
    } else if (type == TEST_CMD) {
        AT_BACK_STR("AT+BTNAME=<namechar>\r\n");
    } else {
        AT_BACK_ERRNO(ERR_AT(AT_ERR_INVAL));
    }

}

_STATIC int save_adv_param(adv_param_t *param, uint8_t ad_set[31], uint8_t ad_len, uint8_t sd_set[31], uint8_t sd_len, uint8_t arg_num)
{
    int ret = 0;

    if (param == NULL || arg_num < 1) {
        return -1;
    }

    if (arg_num >= 1) {
        ret = aos_kv_set("BT_ADV_TYPE", &param->type, sizeof(param->type), 1);

        if (ret) {
            return -1;
        }
    }

    if (arg_num >= 2) {
        ret = aos_kv_set("BT_ADV_INT_MIN", &param->interval_min, sizeof(param->interval_min), 1);

        if (ret) {
            return -1;
        }
    }

    if (arg_num >= 3) {
        ret = aos_kv_set("BT_ADV_INT_MAX", &param->interval_max, sizeof(param->interval_max), 1);

        if (ret) {
            return -1;
        }
    }

    if (arg_num >= 4) {

        ret = aos_kv_set("BT_ADV_AD_LEN", &ad_len, 1, 1);

        if (ret) {
            return -1;
        }

        ret = aos_kv_set("BT_ADV_AD", ad_set, ad_len, 1);

        if (ret) {
            return -1;
        }
    }

    if (arg_num >= 5) {
        ret = aos_kv_set("BT_ADV_SD_LEN", &sd_len, 1, 1);

        if (ret) {
            return -1;
        }

        ret = aos_kv_set("BT_ADV_SD", sd_set, sd_len, 1);

        if (ret) {
            return -1;
        }
    }

    return 0;
}


int ad_data_covert(uint8_t *data, int16_t len, ad_data_t *ad_data)
{
    uint8_t *pdata = data;
    uint8_t ad_len = 0;
    uint8_t ad_num = 0;

    while (len > 0) {
        ad_len = pdata[0];

        if (len >= ad_len + 1) {
            if (ad_data) {
                ad_data[ad_num].len = ad_len - 1;
                ad_data[ad_num].type = pdata[1];
                ad_data[ad_num].data = &pdata[2];
            }

            ad_num++;
            len -= (ad_len + 1);
            pdata += (ad_len + 1);
        } else {
            return 0;
        }
    }

    return ad_num;
}


void at_cmd_bt_adv(char *cmd, int type, char *data)
{
    if (!ble_server.init_flag) {
        AT_BACK_ERRNO(ERR_AT(AT_ERR_AT_SERVICE_NOT_INITIALIZED));
        return;
    }

    if (ble_server.at_config.role == MASTER && ble_server.temp_role_set != SLAVE) {
        AT_BACK_ERRNO(ERR_AT(AT_ERR_CMD_NOT_SUPPORTED));
        return;
    }

    int ret = 0;
    uint8_t ad[31];
    uint8_t sd[31];
    uint8_t interval_min_str[6];
    uint8_t interval_max_str[6];
    int input_num = 0;
    uint8_t onoff;
    uint8_t adv_type;
    uint16_t interval_min;
    uint16_t interval_max;
    uint8_t ad_len = 0;
    uint8_t sd_len = 0;


    if (type == WRITE_CMD) {

        input_num = atserver_scanf("%[^,],%[^,],%[^,],%[^,],%[^,],%[^]", &onoff, &adv_type,  interval_min_str, interval_max_str, ad, sd);

        if (input_num <= 0) {
            AT_BACK_ERRNO(ERR_AT(AT_ERR_INVAL));
            return;
        }

        onoff = onoff - '0';

        if (onoff == 0) {
            ret = ble_server.recv_cb.at_mode_recv_cb(AT_BT_ADV_OFF, NULL, 0, NULL);

            if (ret) {
                AT_BACK_ERRNO(ret);
            } else {
                AT_BACK_OK();
            }

            return;
        }

        if (input_num >= 2) {
            adv_type = adv_type - '0';

            if (adv_type != ADV_IND && adv_type != ADV_NONCONN_IND) {
                goto failed;
            } else {
                ble_server.temp_adv_set.type = adv_type;
            }
        }

        if (input_num >= 3) {
            interval_min = atoi((char *)interval_min_str);

            if (interval_min < AD_INT_MIN_MIN || interval_min > AD_INT_MIN_MAX) {
                goto failed;
            } else {
                ble_server.temp_adv_set.interval_min = interval_min;
            }
        } else {
            ble_server.temp_adv_set.interval_min = AT_BLE_UART_CONFIG_SLAVE(ble_server).param.interval_min;
        }

        if (input_num >= 4) {
            interval_max = atoi((char *)interval_max_str);

            if (interval_max < AD_INT_MAX_MIN || interval_max > AD_INT_MAX_MAX) {
                goto failed;
            } else {
                ble_server.temp_adv_set.interval_max = interval_max;
            }
        } else {
            ble_server.temp_adv_set.interval_max = AT_BLE_UART_CONFIG_SLAVE(ble_server).param.interval_max;
        }

        if (input_num >= 5) {
            ad_len = str2hex(ble_server.temp_ad_set, (char *)ad, sizeof(ble_server.temp_ad_set));
        } else {
            ble_server.temp_adv_set.ad = AT_BLE_UART_CONFIG_SLAVE(ble_server).param.ad;
            ble_server.temp_adv_set.ad_num = AT_BLE_UART_CONFIG_SLAVE(ble_server).param.ad_num;
        }

        if (input_num >= 6) {
            sd_len = str2hex(ble_server.temp_sd_set, (char *)sd, sizeof(ble_server.temp_sd_set));
        } else {
            ble_server.temp_adv_set.sd = AT_BLE_UART_CONFIG_SLAVE(ble_server).param.sd;
            ble_server.temp_adv_set.sd_num = AT_BLE_UART_CONFIG_SLAVE(ble_server).param.sd_num;
        }

        if (input_num == 1) {
            ret = ble_server.recv_cb.at_mode_recv_cb(AT_BT_ADV_ON, NULL, 0, NULL);

            if (ret < 0) {
                AT_BACK_ERRNO(ret);
            } else {
                AT_BACK_OK();
            }

            return;
        } else {
            if (ad_len > 0) {
                ble_server.temp_adv_set.ad_num = ad_data_covert(ble_server.temp_ad_set, ad_len, NULL);

                if (ad_out) {
                    free(ad_out);
                    ad_out = NULL;
                }

                ad_out = malloc(sizeof(ad_data_t) * ble_server.temp_adv_set.ad_num);
                ad_data_covert(ble_server.temp_ad_set, ad_len, ad_out);
                ble_server.temp_adv_set.ad = ad_out;
            }

            if (sd_len > 0) {
                ble_server.temp_adv_set.sd_num = ad_data_covert(ble_server.temp_sd_set, sd_len, NULL);

                if (sd_out) {
                    free(sd_out);
                    sd_out = NULL;
                }

                sd_out = malloc(sizeof(ad_data_t) * ble_server.temp_adv_set.sd_num);
                ad_data_covert(ble_server.temp_sd_set, sd_len, sd_out);
                ble_server.temp_adv_set.sd = sd_out;
            }

            ret = ble_server.recv_cb.at_mode_recv_cb(AT_BT_ADV_ON, (void *)&ble_server.temp_adv_set, sizeof(ble_server.temp_adv_set), NULL);

            if (ret < 0) {
                AT_BACK_ERRNO(ret);
                return;
            }

            ret = save_adv_param(&ble_server.temp_adv_set, ble_server.temp_ad_set, ad_len, ble_server.temp_sd_set, sd_len, input_num - 1);

            if (ret) {
                AT_BACK_ERRNO(ERR_AT(AT_ERR_BT_KV_SAVE_PARAM));
            } else {
                AT_BACK_OK();
            }

            return;
        }

failed:
        AT_BACK_ERRNO(ERR_AT(AT_ERR_INVAL));
        return;

    } else if (type == TEST_CMD) {
        AT_BACK_STR("AT+BTADV=<on_off>,[type],[interval_min],[interval_max],[ad],[sd]\r\n");
    } else {
        AT_BACK_ERRNO(ERR_AT(AT_ERR_INVAL));
    }
}


void at_cmd_bt_adv_def(char *cmd, int type, char *data)
{
    if (!ble_server.init_flag) {
        AT_BACK_ERRNO(ERR_AT(AT_ERR_AT_SERVICE_NOT_INITIALIZED));
        return;
    }

    if (ble_server.at_config.role == MASTER && ble_server.temp_role_set != SLAVE) {
        AT_BACK_ERRNO(ERR_AT(AT_ERR_CMD_NOT_SUPPORTED));
        return;
    }

    uint8_t adv_def_on = 1;
    int ret;

    if (type == WRITE_CMD) {
        atserver_scanf("%[0-9]", &adv_def_on);
        adv_def_on = adv_def_on - '0';

        if (adv_def_on != ADV_OFF && adv_def_on != ADV_ON) {
            AT_BACK_ERRNO(ERR_AT(AT_ERR_INVAL));
            return;
        }

        ret = aos_kv_set("BT_ADV_DEF", &adv_def_on, sizeof(adv_def_on), 1);

        if (ret) {
            AT_BACK_ERRNO(ERR_AT(AT_ERR_BT_KV_SAVE_PARAM));
        } else {
            AT_BACK_OK();
        }
    } else if (type == READ_CMD) {
        AT_BACK_RET_INT_HEAD(cmd, AT_BLE_UART_CONFIG_SLAVE(ble_server).adv_def_on);
    } else if (type == TEST_CMD) {
        AT_BACK_STR("AT+BTAUTOADV=<def_on>\r\n");
    } else {
        AT_BACK_ERRNO(ERR_AT(AT_ERR_INVAL));
    }
}


_STATIC int save_conn_param(dev_addr_t dev)
{
    int num = 0;
    uint8_t ret = 0;
    char key_name[40];

    master_load *master_conf = &AT_BLE_UART_CONFIG_MASTER(ble_server);

    if (master_conf->auto_conn_num > 0) {
        for (int i = 0; i < master_conf->auto_conn_num ; i++) {
            if (!memcmp((void *)&dev, &master_conf->auto_conn_info[i], sizeof(dev_addr_t))) {
                return 0;
            }
        }
    }

    num = length_temp  % AUTO_CONN_BT_MAX;

    snprintf(key_name, sizeof(key_name), "BT_CONN_%d", num);
    ret = aos_kv_set(key_name, &dev, sizeof(dev), 1);

    if (ret == 0) {
        length_temp++;

        if (length_temp >= 2 * AUTO_CONN_BT_MAX) {
            length_temp = length_temp - AUTO_CONN_BT_MAX;
        }

        ret = aos_kv_set("BT_CONN_NUM", &length_temp, sizeof(length_temp), 1);

        if (ret) {
            ret = aos_kv_del(key_name);

            if (ret) {
                length_temp --;
                return -1;
            }
        }
    } else {
        return -1;
    }

    return 0;
}

_STATIC int char2hex(const char *c, uint8_t *x)
{
    if (*c >= '0' && *c <= '9') {
        *x = *c - '0';
    } else if (*c >= 'a' && *c <= 'f') {
        *x = *c - 'a' + 10;
    } else if (*c >= 'A' && *c <= 'F') {
        *x = *c - 'A' + 10;
    } else {
        return -1;
    }

    return 0;
}

_STATIC int str2bt_addr(const char *str, dev_addr_t *addr)
{
    uint8_t i, j;
    uint8_t tmp;

    if (strlen(str) != 17) {
        return -1;
    }

    for (i = 5, j = 1; *str != '\0' ; str++, j++) {
        if (!(j % 3) && (*str != ':')) {
            return -1;
        } else if (*str == ':') {
            i--;
            continue;
        }

        addr->val[i] = addr->val[i] << 4;

        if (char2hex(str, &tmp) < 0) {
            return -1;
        }

        addr->val[i] |= tmp;
    }

    return 0;
}

void at_cmd_bt_conn(char *cmd, int type, char *data)
{
    if (!ble_server.init_flag || ble_server.recv_cb.at_mode_recv_cb == NULL) {
        AT_BACK_ERRNO(ERR_AT(AT_ERR_AT_SERVICE_NOT_INITIALIZED));
        return;
    }

    if (ble_server.at_config.role == SLAVE && ble_server.temp_role_set != MASTER) {
        AT_BACK_ERRNO(ERR_AT(AT_ERR_CMD_NOT_SUPPORTED));
        return;
    }

    int ret;
    uint8_t input_num;
    uint8_t addr_type;
    uint8_t adr_str[20];
    dev_addr_t dev;

    if (type == WRITE_CMD) {
        input_num = atserver_scanf("%[^,],%[0-9]", adr_str, &addr_type);

        if (input_num != 2) {
            AT_BACK_ERRNO(ERR_AT(AT_ERR_INVAL));
            return;
        }

        addr_type = addr_type - '0';

        if (addr_type != DEV_ADDR_LE_RANDOM && addr_type != DEV_ADDR_LE_PUBLIC) {
            AT_BACK_ERRNO(ERR_AT(AT_ERR_INVAL));
            return;
        }

        ret = str2bt_addr((char *)adr_str, &dev);

        if (ret) {
            AT_BACK_ERRNO(ERR_AT(AT_ERR_INVAL));
            return;
        }

        dev.type = addr_type;
    } else if (type == TEST_CMD) {
        AT_BACK_STR("AT+BTCONN=<mac>,<mac_type>\r\n");
        return;
    } else {
        AT_BACK_ERRNO(ERR_AT(AT_ERR_INVAL));
        return;
    }

    if (ble_server.at_config.role == MASTER) {
        ret = ble_server.recv_cb.at_mode_recv_cb(AT_BT_CONNECT, (void *)&dev, sizeof(dev), NULL);

        if (ret) {
            AT_BACK_ERRNO(ret);
            return;
        }
    }

    ret = save_conn_param(dev);

    if (ret) {
        AT_BACK_ERRNO(ERR_AT(AT_ERR_BT_KV_SAVE_PARAM));
    } else {
        AT_BACK_OK();
    }
}

void at_cmd_bt_disconn(char *cmd, int type, char *data)
{
    int ret;

    if (!ble_server.init_flag || ble_server.recv_cb.at_mode_recv_cb == NULL) {
        AT_BACK_ERRNO(ERR_AT(AT_ERR_AT_SERVICE_NOT_INITIALIZED));
        return;
    }

    if (type == EXECUTE_CMD) {
        ret = ble_server.recv_cb.at_mode_recv_cb(AT_BT_DISCONNECT, NULL, 0, NULL);

        if (ret) {
            AT_BACK_ERRNO(ret);
            return;
        } else {
            AT_BACK_OK();
        }
    } else {
        AT_BACK_ERRNO(ERR_AT(AT_ERR_INVAL));
    }

}

void at_cmd_bt_conn_def(char *cmd, int type, char *data)
{
    if (!ble_server.init_flag) {
        AT_BACK_ERRNO(ERR_AT(AT_ERR_AT_SERVICE_NOT_INITIALIZED));
        return;
    }

    if (ble_server.at_config.role == SLAVE &&  ble_server.temp_role_set != MASTER) {
        AT_BACK_ERRNO(ERR_AT(AT_ERR_CMD_NOT_SUPPORTED));
        return;
    }

    int ret;
    uint8_t connect_def_on = 1;
    uint8_t input_num;

    if (type == WRITE_CMD) {

        input_num = atserver_scanf("%[0-9]", &connect_def_on);

        if (input_num != 1) {
            AT_BACK_ERRNO(ERR_AT(AT_ERR_INVAL));
            return;
        }

        connect_def_on = connect_def_on - '0';

        if (connect_def_on != CONN_AUTO_ON && connect_def_on != CONN_AUTO_OFF) {
            AT_BACK_ERRNO(ERR_AT(AT_ERR_INVAL));
            return;
        }

        ret = aos_kv_set("BT_CONNAUTO", &connect_def_on, sizeof(connect_def_on), 1);

        if (ret) {
            AT_BACK_ERRNO(ERR_AT(AT_ERR_BT_KV_SAVE_PARAM));
        } else {
            AT_BACK_OK();
        }
    } else if (type == READ_CMD) {
        AT_BACK_RET_INT_HEAD(cmd, AT_BLE_UART_CONFIG_MASTER(ble_server).conn_def_on);
    } else if (type == TEST_CMD) {
        AT_BACK_STR("AT+BTAUTOCONN=<def_on>\r\n");
    } else {
        AT_BACK_ERRNO(ERR_AT(AT_ERR_INVAL));
    }

}


_STATIC void tx_end(int err, void *cb_data)
{
    if (err) {
        AT_BACK_ERRNO(ERR_AT(AT_ERR_BT_TX_FAILED));
    } else {
        AT_BACK_OK();
    }
}

static at_bt_uart_send_cb tx_cb = {
    .start = NULL,
    .end = tx_end,
};


void at_cmd_bt_tx(char *cmd, int type, char *data)
{
    if (!ble_server.init_flag || ble_server.recv_cb.at_mode_recv_cb == NULL) {
        AT_BACK_ERRNO(ERR_AT(AT_ERR_AT_SERVICE_NOT_INITIALIZED));
        return;
    }

    uint16_t data_len;
    int ret;

    if (type == WRITE_CMD) {
        data_len = strlen(data);////

        if (data_len == 0 || data_len > TX_MAX_LEN) {
            AT_BACK_ERRNO(ERR_AT(AT_ERR_BT_TX_LEN));
        } else {
            ret = ble_server.recv_cb.at_mode_recv_cb(AT_BT_TX_DATA, data, data_len, &tx_cb);

            if (ret < 0) {
                AT_BACK_ERRNO(ERR_AT(AT_ERR_BT_TX_FAILED));
            }
        }
    } else if (type == TEST_CMD) {
        AT_BACK_STR("AT+BTTX=<data>");
    } else {
        AT_BACK_ERRNO(ERR_AT(AT_ERR_INVAL));
    }
}


void at_cmd_bt_role(char *cmd, int type, char *data)
{
    if (!ble_server.init_flag) {
        AT_BACK_ERRNO(ERR_AT(AT_ERR_AT_SERVICE_NOT_INITIALIZED));
        return;
    }

    uint8_t role;
    int ret;

    if (type == WRITE_CMD) {
        atserver_scanf("%[0-9]", &role);
        role = role - '0';

        if (role != MASTER && role != SLAVE) {
            AT_BACK_ERRNO(ERR_AT(AT_ERR_INVAL));
        } else {
            ret = aos_kv_set("BT_ROLE", &role, sizeof(role), 1);

            if (ret) {
                AT_BACK_ERRNO(ERR_AT(AT_ERR_BT_KV_SAVE_PARAM));
            } else {
                AT_BACK_OK();
            }

            ble_server.temp_role_set = role;
        }
    } else if (type == READ_CMD) {
        AT_BACK_RET_INT_HEAD(cmd, ble_server.at_config.role);
    } else if (type == TEST_CMD) {
        AT_BACK_STR("AT+BTROLE=<role>\r\n");
    } else {
        AT_BACK_ERRNO(ERR_AT(AT_ERR_INVAL));
    }
}

void at_cmd_bt_reboot(char *cmd, int type, char *data)
{
    if (!ble_server.init_flag) {
        AT_BACK_ERRNO(ERR_AT(AT_ERR_AT_SERVICE_NOT_INITIALIZED));
        return;
    }

    if (type == EXECUTE_CMD) {
        aos_reboot();
    }
}

void at_cmd_bt_rst(char *cmd, int type, char *data)
{
    if (!ble_server.init_flag) {
        AT_BACK_RET_INT_HEAD(cmd, ERR_AT(AT_ERR_BT_KV_UNLOAD));
        return;
    }

    int ret;

    if (type == EXECUTE_CMD) {
        extern int at_ble_conf_erase();
        ret = at_ble_conf_erase();

        if (ret) {
            AT_BACK_ERRNO(ret);
            return;
        } else {
            AT_BACK_OK();
        }
    }

    aos_reboot();
}

void at_cmd_bt_sleep_mode(char *cmd, int type, char *data)
{
    if (!ble_server.init_flag) {
        AT_BACK_ERRNO(ERR_AT(AT_ERR_AT_SERVICE_NOT_INITIALIZED));
        return;
    }

    uint8_t sleep_mode;
    int ret;

    if (type == WRITE_CMD) {
        atserver_scanf("%[0-9]", &sleep_mode);
        sleep_mode = sleep_mode - '0';

        if (sleep_mode > STANDBY) {
            AT_BACK_ERRNO(ERR_AT(AT_ERR_INVAL));
            return;
        }

        ret = aos_kv_set("BT_SLEEP_MODE", &sleep_mode, sizeof(sleep_mode), 1);

        if (ret) {
            AT_BACK_ERRNO(ERR_AT(AT_ERR_BT_KV_SAVE_PARAM));
            return;
        } else {
            AT_BACK_OK();
        }
    } else if (type == READ_CMD) {
        AT_BACK_RET_INT_HEAD(cmd, ble_server.at_config.sleep_mode);
    } else if (type == TEST_CMD) {
        AT_BACK_STR("AT+BTSLEEP=<sleep mode>\r\n");
    } else {
        AT_BACK_ERRNO(ERR_AT(AT_ERR_INVAL));
    }
}




_STATIC inline int check_tx_power_value(int set_pow)
{
    for (int i = 0; i < sizeof(tx_power_table) / sizeof(tx_power_table[0]); i++) {

        if (set_pow == tx_power_table[i].set_power) {
            return tx_power_table[i].set_value;
        }
    }

    return -1;
}


_STATIC inline int get_tx_power(uint8_t tx_set)
{

    for (int i = 0; i < sizeof(tx_power_table) / sizeof(tx_power_table[0]); i++) {
        if (tx_set == tx_power_table[i].set_value) {
            return tx_power_table[i].set_power;
        }
    }

    return GET_TX_POWER_FAILED;
}

void at_cmd_bt_tx_power(char *cmd, int type, char *data)
{
    if (!ble_server.init_flag) {
        AT_BACK_ERRNO(ERR_AT(AT_ERR_AT_SERVICE_NOT_INITIALIZED));
        return;
    }

    int tx_power;
    int tx_set;
    int ret;

    if (type == WRITE_CMD) {
        tx_power = atoi(data);
        tx_set = check_tx_power_value(tx_power);

        if (tx_set < 0) {
            AT_BACK_ERRNO(ERR_AT(AT_ERR_INVAL));
            return;
        }

        ret = aos_kv_setint("BT_TXPOWER", tx_set);

        if (ret) {
            AT_BACK_ERRNO(ERR_AT(AT_ERR_BT_KV_SAVE_PARAM));
            return;
        } else {
            AT_BACK_OK();
        }
    } else if (type == READ_CMD) {
        tx_power = get_tx_power(ble_server.at_config.tx_power);
        AT_BACK_RET_INT_HEAD(cmd, tx_power);
    } else if (type == TEST_CMD) {
        AT_BACK_STR("AT+BTTXPOW=<tx_power>\r\n");
    } else {
        AT_BACK_ERRNO(ERR_AT(AT_ERR_INVAL));
    }
}

int read_public_addr(dev_addr_t *addr)
{
    if (addr == NULL) {
        return -1;
    }

    addr->type = 0;

#ifdef CONFIG_DUT_SERVICE_ENABLE
    return dut_hal_mac_get(addr->val);
#else
    return 0;
#endif
}

_STATIC int set_public_addr(dev_addr_t addr)
{
#ifdef CONFIG_DUT_SERVICE_ENABLE
    return dut_hal_mac_store(addr.val);
#else
    return 0;
#endif
}

void at_cmd_bt_mac(char *cmd, int type, char *data)
{
    if (!ble_server.init_flag) {
        AT_BACK_ERRNO(ERR_AT(AT_ERR_AT_SERVICE_NOT_INITIALIZED));
        return;
    }

    dev_addr_t dev;
    uint8_t input_num;
    uint8_t addr_type;
    uint8_t addr_str[20];
    uint8_t dev_str[27];
    int ret = 0;

    if (type == WRITE_CMD) {
        input_num = atserver_scanf("%[^,],%[0-9]", addr_str, &addr_type);

        if (input_num != 2) {
            AT_BACK_ERRNO(ERR_AT(AT_ERR_INVAL));
            return;
        }

        addr_type = addr_type - '0';

        if (addr_type != DEV_ADDR_LE_RANDOM && addr_type != DEV_ADDR_LE_PUBLIC) {
            AT_BACK_ERRNO(ERR_AT(AT_ERR_INVAL));
            return;
        }

        ret = str2bt_addr((char *)addr_str, &dev);

        if (ret) {
            AT_BACK_ERRNO(ERR_AT(AT_ERR_INVAL));
            return;
        }

        if (addr_type == DEV_ADDR_LE_PUBLIC) {
            ret = set_public_addr(dev);

            if (ret) {
                AT_BACK_ERRNO(ERR_AT(AT_ERR_INVAL));
                return;
            }

            ret = aos_kv_set("BT_ADDR_TYPE", &addr_type, 1, 1);

            if (ret) {
                AT_BACK_ERRNO(ERR_AT(AT_ERR_BT_KV_SAVE_PARAM));
                return;
            }
        } else {
            if ((dev.val[5] & 0XC0) != 0XC0) {
                AT_BACK_ERRNO(ERR_AT(AT_ERR_INVAL));
                return;
            }

            ret = aos_kv_set("BT_ADDR_TYPE", &addr_type, 1, 1);

            if (ret) {
                AT_BACK_ERRNO(ERR_AT(AT_ERR_BT_KV_SAVE_PARAM));
                return;
            }

            ret = aos_kv_set("BT_MAC", dev.val, sizeof(dev.val), 1);

            if (ret) {
                AT_BACK_ERRNO(ERR_AT(AT_ERR_BT_KV_SAVE_PARAM));
                return;
            }
        }

        AT_BACK_OK();
    } else if (type == READ_CMD) {
        memset(dev.val, 0x00, sizeof(dev.val));
        if (ble_server.at_config.addr.type  == 1) {
            snprintf((char *)dev_str, sizeof(dev_str), "%02x:%02x:%02x:%02x:%02x:%02x,1", \
                     ble_server.at_config.addr.val[5],  ble_server.at_config.addr.val[4], ble_server.at_config.addr.val[3], ble_server.at_config.addr.val[2], ble_server.at_config.addr.val[1], ble_server.at_config.addr.val[0]);
            AT_BACK_RET_OK_HEAD(cmd, dev_str);
        } else {
            ret = read_public_addr(&dev);

            if (ret) {
                AT_BACK_ERRNO(ERR_AT(AT_ERR_BT_KV_SAVE_PARAM));
                return;
            }

            snprintf((char *)dev_str, sizeof(dev_str), "%02x:%02x:%02x:%02x:%02x:%02x,0", \
                     dev.val[5], dev.val[4], dev.val[3], dev.val[2], dev.val[1], dev.val[0]);
            AT_BACK_RET_OK_HEAD(cmd, dev_str);
        }
    } else if (type == TEST_CMD) {
        AT_BACK_STR("AT+BTMAC=<mac>,<mac type>\r\n");
    } else {
        AT_BACK_ERRNO(ERR_AT(AT_ERR_INVAL));
    }
}


void at_cmd_bt_baud(char *cmd, int type, char *data)
{
    if (!ble_server.init_flag) {
        AT_BACK_ERRNO(ERR_AT(AT_ERR_AT_SERVICE_NOT_INITIALIZED));
        return;
    }

    int baud;
    int ret = 0;

    if (type == WRITE_CMD) {
        baud = atoi(data);

        if (baud != 9600 && baud != 115200 && baud != 250000 && baud != 512000 && baud != 600000) {
            AT_BACK_ERRNO(ERR_AT(AT_ERR_INVAL));
            return;
        }

        ret = aos_kv_setint("BT_BAUD", baud);

        if (ret) {
            AT_BACK_ERRNO(ERR_AT(AT_ERR_BT_KV_SAVE_PARAM));
        } else {
            AT_BACK_OK();
        }
    } else if (type == READ_CMD) {
        AT_BACK_RET_INT_HEAD(cmd, ble_server.at_config.baud);
    } else if (type == TEST_CMD) {
        AT_BACK_STR("AT+BAUD=<baud>\r\n");
    } else {
        AT_BACK_ERRNO(ERR_AT(AT_ERR_INVAL));
    }
}

void at_cmd_bt_find(char *cmd, int type, char *data)
{

    if (!ble_server.init_flag) {
        AT_BACK_ERRNO(ERR_AT(AT_ERR_AT_SERVICE_NOT_INITIALIZED));
        return;
    }

    if (ble_server.at_config.role == SLAVE) {
        AT_BACK_ERRNO(ERR_AT(AT_ERR_CMD_NOT_SUPPORTED));
        return;
    }

    int ret;

    if (type == EXECUTE_CMD) {
        ret = ble_server.recv_cb.at_mode_recv_cb(AT_BT_DEV_FIND, NULL, 0, NULL);

        if (ret < 0) {
            AT_BACK_ERRNO(ret);
        } else {
            AT_BACK_OK();
        }
    } else {
        AT_BACK_ERRNO(ERR_AT(AT_ERR_INVAL));
    }
}


void at_cmd_bt_conn_update(char *cmd, int type, char *data)
{
    if (!ble_server.init_flag) {
        AT_BACK_ERRNO(ERR_AT(AT_ERR_AT_SERVICE_NOT_INITIALIZED));
        return;
    }

    conn_param_t param;
    uint8_t input_num;
    uint8_t store_param;
    uint8_t interval_min_str[10];
    uint8_t interval_max_str[10];
    uint8_t lantency_str[10];
    uint8_t timeout_str[10];
    int ret;

    if (type == WRITE_CMD) {
        input_num = atserver_scanf("%[^,],%[^,],%[^,],%[^,],%[^]", &store_param, interval_min_str, interval_max_str, lantency_str, timeout_str);

        if (input_num != 5) {
            AT_BACK_ERRNO(ERR_AT(AT_ERR_INVAL));
            return;
        }

        param.interval_min = atoi((char *)interval_min_str);
        param.interval_max = atoi((char *)interval_max_str);
        param.latency = atoi((char *)lantency_str);
        param.timeout = atoi((char *)timeout_str);

        if (!ble_stack_check_conn_params(&param)) {
            AT_BACK_ERRNO(ERR_AT(AT_ERR_INVAL));
            return;
        }

        ret = ble_server.recv_cb.at_mode_recv_cb(AT_BT_CONNECT_UPDATE, (void *)&param, sizeof(param), NULL);

        if (ret) {
            AT_BACK_ERRNO(ret);
            return;
        }

        store_param = store_param - '0';
        ret = aos_kv_set("CONN_UPDATE_ON", &store_param, sizeof(store_param), 1);

        if (ret) {
            AT_BACK_ERRNO(ERR_AT(AT_ERR_BT_KV_SAVE_PARAM));
            return;
        }

        if (store_param) {
            ret = aos_kv_set("CONN_PARAM", &param, sizeof(param), 1);

            if (ret) {
                AT_BACK_ERRNO(ERR_AT(AT_ERR_BT_KV_SAVE_PARAM));
                return;
            }
        }

        AT_BACK_OK();
    }  else if (type == TEST_CMD) {
        AT_BACK_STR("AT+BTCONNUPDATE=<def_on><interval_min><interval_max><lantency><timeout>\r\n");
    } else {
        AT_BACK_ERRNO(ERR_AT(AT_ERR_INVAL));
    }
}

void at_ble_uartmode_recv(rvm_dev_t *dev)
{
    int ret;

    ret = rvm_hal_uart_recv(dev, ble_server.uart_buffer, AT_BLE_UART_BUFFER_SIZE, 25);

    if (ret > 0) {
        ble_server.recv_cb.uart_mode_recv_cb((char *)ble_server.uart_buffer, ret, NULL);
    }

    return;
}

at_server_handler *at_ble_init()
{
    int ret;

    extern int at_ble_conf_load(at_conf_load * conf_load);
    ret = at_ble_conf_load(&ble_server.at_config);

    if (ret < 0) {
        return NULL;
    }

    ble_server_handler.at_config = &ble_server.at_config;
    ble_server.temp_role_set = NO_ROLE;
    ble_server.init_flag = 1;

    return &ble_server_handler;

}

int at_ble_event_register(const at_ble_cb cb_conf)
{
    if (!ble_server.init_flag || !cb_conf.at_mode_recv_cb || !cb_conf.uart_mode_recv_cb) {
        return -1;
    }

    ble_server.recv_cb = cb_conf;
    return 0;
}

#endif