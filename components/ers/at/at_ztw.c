/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#if defined(CONFIG_CHIP_ZX297100)
#include <stdio.h>
#include <stdlib.h>
#include <aos/log.h>
#include "at_internal.h"
#include "oss_nv.h"

#if defined(CONFIG_AMT)
#include "amt_adapter.h"
#include "amt.h"
#endif

char *TAG = "at_ztw";

static void _swap_buf(char *buf, uint32_t len)
{
    char tmp, i;

    for (i = 0; i < len / 2; i ++) {
        tmp = *(buf + i);
        *(buf + i) = *(buf + len - i - 1);
        *(buf + len - i - 1) = tmp;
    }
}

void at_cmd_znvget(char *cmd, int type, char *data)
{
    if (type == WRITE_CMD) {
        int ret;
        char addr_str[10];
        char data_str[128];
        char read_data[64];
        uint32_t data_len = 0;
        void *p[] = {addr_str, &data_len};

        /* parse param */
        ret = at_parse_param_safe("%10s,%4d", (char *)data, p);

        if (ret < 0 || ret != 2) {
            LOGE(TAG, "parse");
            AT_BACK_RET_ERR(cmd, AT_ERR_INVAL);
            return;
        }

        if (data_len < 0 || data_len > 64) {
            LOGE(TAG, "len");
            AT_BACK_RET_ERR(cmd, AT_ERR_INVAL);
            return;
        }

        /* string_hex convert */
        if (4 != hexstr2bytes(addr_str)) {
            LOGE(TAG, "len");
            AT_BACK_RET_ERR(cmd, AT_ERR_INVAL);
            return;
        }

        /* convert addr to little endian */
        _swap_buf(addr_str, 4);

        ret = oss_nv_read(*(uint32_t *)addr_str, read_data,  data_len);

        if (ret < 0) {
            LOGE(TAG, "read");
            AT_BACK_RET_ERR(cmd, AT_ERR_INVAL);
            return;
        }

        memset(data_str , 0, 128);

        bytes2hexstr(data_str, sizeof(data_str), (uint8_t *)read_data, data_len);

        AT_BACK_RET_OK(cmd, data_str);

        return;
    } else {
        LOGE(TAG,"run %s %d", cmd, type);
    }
}

void at_cmd_znvset(char *cmd, int type, char *data)
{
    if (type == WRITE_CMD) {
        int ret;
        char addr_str[10];
        char data_str[128];
        uint32_t data_len = 0;
        void *p[] = {addr_str, &data_len, data_str};

        /* parse param */
        ret = at_parse_param_safe("%10s,%4d,%128s", (char *)data, p);

        if (ret < 0 || ret != 3) {
            LOGE(TAG, "parse");
            AT_BACK_RET_ERR(cmd, AT_ERR_INVAL);
            return;
        }

        if (data_len < 0 || data_len > 128) {
            LOGE(TAG, "len");
            AT_BACK_RET_ERR(cmd, AT_ERR_INVAL);
            return;
        }

        /* string_hex convert */
        if (4 != hexstr2bytes(addr_str)) {
            LOGE(TAG, "len");
            AT_BACK_RET_ERR(cmd, AT_ERR_INVAL);
            return;
        }

        /* convert addr to little endian */
        _swap_buf(addr_str, 4);

        /* string_hex convert */
        if (data_len != hexstr2bytes(data_str)) {
            LOGE(TAG, "len");
            AT_BACK_RET_ERR(cmd, AT_ERR_INVAL);
            return;
        }

        ret = oss_nv_write(*(uint32_t *)addr_str, data_str, data_len);

        if (ret < 0) {
            LOGE(TAG, "write");
            AT_BACK_RET_ERR(cmd, AT_ERR_INVAL);
            return;
        }

        AT_BACK_OK();
        return;
    } else {
        LOGE(TAG,"run %s %d", cmd, type);
    }
}

void at_cmd_ramdump(char *cmd, int type, char *data)
{
    if (type == EXECUTE_CMD) {
        oss_assert_fail(0);
    } else {
        LOGE(TAG,"run %s %d", cmd, type);
    }
}

#ifdef CONFIG_YOC_LPM
void at_cmd_extrtc(char *cmd, int type, char *data)
{
    if (type == WRITE_CMD) {
        int32_t sleep_time;
        sleep_time = atoi((char *)data);

        extern void pm_si_set_rtc_wakeup_time(uint32_t sleep_time);
        pm_si_set_rtc_wakeup_time(sleep_time);

        AT_BACK_OK();
    } else {
        LOGE(TAG,"run %s %d", cmd, type);
    }
}

void at_cmd_quick_lpm(char *cmd, int type, char *data)
{
    if (type == EXECUTE_CMD) {
        AT_BACK_OK();

        extern void pm_si_quick_sby_enter();
        pm_si_quick_sby_enter();
    } else {
        LOGE(TAG,"run %s %d", cmd, type);
    }
}
#endif

#if defined(CONFIG_AMT)

#define BOOT_FLAG_BASE      (0x1003FE00)
/* AT+ZFALG=BOOT,1/0 */
void at_cmd_zflag(char *cmd, int type, char *data)
{
    if (type == WRITE_CMD) {
        int ret;
        uint8_t  dl_flag;
        char    flag[10];
        char    boot_key[10];
        void *p[] = {boot_key, flag};

        /* parse param */
        ret = at_parse_param_safe("%10s,%10s", (char *)data, p);

        if (ret < 0 || ret != 2) {
            LOGE(TAG, "parse");
            AT_BACK_RET_ERR(cmd, AT_ERR_INVAL);
            return;
        }

        if (strcmp(boot_key, "BOOT")) {
            LOGE(TAG, "boot");
            AT_BACK_RET_ERR(cmd, AT_ERR_INVAL);
            return;
        }

        if (strcmp(flag, "OPEN") == 0) {
            dl_flag = 0;
            ret = amt_eflash_write(BOOT_FLAG_BASE, (u8 *)(&dl_flag), 1);

            if (ret < 0) {
                LOGE(TAG, "write");
                AT_BACK_RET_ERR(cmd, AT_ERR_INVAL);
                return;
            }
        } else if (strcmp(flag, "CLOSE") == 0) {
            dl_flag = 1;
            ret = amt_eflash_write(BOOT_FLAG_BASE, (u8 *)(&dl_flag), 1);

            if (ret < 0) {
                LOGE(TAG, "write");
                AT_BACK_RET_ERR(cmd, AT_ERR_INVAL);
                return;
            }
        } else if (strcmp(flag, "?") == 0) {
            ret = amt_eflash_read(BOOT_FLAG_BASE, (u8 *)(&dl_flag), 1);

            if (ret < 0) {
                LOGE(TAG, "read");
                AT_BACK_RET_ERR(cmd, AT_ERR_INVAL);
                return;
            }

            if (dl_flag == 0) {
                AT_BACK_CMD("+ZFLAG: BOOT,OPEN\r\n");
            } else if (dl_flag == 1) {
                AT_BACK_CMD("+ZFLAG: BOOT,CLOSE\r\n");
            } else {
                LOGE(TAG, "read");
                AT_BACK_RET_ERR(cmd, AT_ERR_INVAL);
                return;
            }

        } else {
            LOGE(TAG, "flag");
            AT_BACK_RET_ERR(cmd, AT_ERR_INVAL);
            return;
        }

        AT_BACK_OK();
        return;
    } else {
        LOGE(TAG,"run %s %d", cmd, type);
    }
}

void at_cmd_amtdemo(char *cmd, int type, char *data)
{
    if (type == WRITE_CMD) {
        int ret;
        int  p1 = 0;
        char p2[10] = {0};
        void *p[] = {&p1, p2};

        /* parse param */
        ret = at_parse_param_safe("%d,%10s", (char *)data, p);

        if (ret < 0 || ret != 2) {
            LOGE(TAG, "parse");
            AT_BACK_RET_ERR(cmd, AT_ERR_INVAL);
            return;
        }

        atserver_send("\r\n+AMTDEMO: %d,\"%s\"\r\nOK\r\n", p1, p2);

        return;
    } else {
        LOGE(TAG,"run %s %d", cmd, type);
    }
}

void at_cmd_boardnum(char *cmd, int type, char *data)
{
    if (type == READ_CMD) {
        char buf[AMT_COMM_BoardNum_SIZE] = {0};

        if (0 != amt_read_nv_item(BoardNum_NVItem, (u8 *)buf, AMT_COMM_BoardNum_SIZE - 1)) {
            AT_BACK_ERR();
            return;
        }

        atserver_send("\r\n+BOARDNUM: %s\r\nOK\r\n", buf);

        return;
    } else {
        LOGE(TAG,"run %s %d", cmd, type);
    }
}

void at_cmd_msn(char *cmd, int type, char *data)
{
    if (type == READ_CMD) {
        char buf[AMT_COMM_MSN_SIZE] = {0};

        if (0 != amt_read_nv_item(MSN_NVItem, (u8 *)buf, AMT_COMM_MSN_SIZE - 1)) {
            AT_BACK_ERR();
            return;
        }

        atserver_send("\r\n+MSN: %s\r\nOK\r\n", buf);

        return;
    } else {
        LOGE(TAG,"run %s %d", cmd, type);
    }
}

void at_cmd_chip_platform(char *cmd, int type, char *data)
{
    if (type == READ_CMD) {
        atserver_send("\r\n+PLATFORM: %s\r\nOK\r\n", "2");

        return;
    } else {
        LOGE(TAG,"run %s %d", cmd, type);
    }
}

void at_cmd_prodtest(char *cmd, int type, char *data)
{
    if (type == WRITE_CMD) {
        int ret;
        int  p1 = 0;

        u8 TestInfo = 0;
        void *p[] = {&p1};

        /* parse param */
        ret = at_parse_param_safe("%d", (char *)data, p);

        if (ret < 0 || ret != 1) {
            LOGE(TAG, "parse");
            AT_BACK_RET_ERR(cmd, AT_ERR_INVAL);
            return;
        }

        if (0 != amt_read_test_flag_item(p1, &TestInfo)) {
            AT_BACK_ERR();
            return;
        }

        atserver_send("\r\n+PRODTEST: %c\r\nOK\r\n", p1);

        return;
    } else {
        LOGE(TAG,"run %s %d", cmd, type);
    }
}

void at_cmd_rtestinfo(char *cmd, int type, char *data)
{
    if (type == READ_CMD) {
        char buf[AMT_COMM_TestInfo_SIZE + 1] = {0};

        if (0 != amt_read_nv_item(TestInfo_NVItem, (u8 *)buf, AMT_COMM_TestInfo_SIZE)) {
            AT_BACK_ERR();
            return;
        }

        atserver_send("\r\n+RTESTINFO:%s\r\nOK\r\n", buf);

        return;
    } else {
        LOGE(TAG,"run %s %d", cmd, type);
    }
}

void at_cmd_zversiontype(char *cmd, int type, char *data)
{
    if (type == WRITE_CMD) {
        int ret;
        int  version_type = 0;
        u8  mode_value = 1; /* 1 normal mode; 5 amt mode */

        void *p[] = {&version_type};

        /* parse param */
        ret = at_parse_param_safe("%d", (char *)data, p);

        if (ret < 0 || ret != 1) {
            LOGE(TAG, "parse");
            AT_BACK_RET_ERR(cmd, AT_ERR_INVAL);
            return;
        }

        if (version_type == 0) { //normal mode
            if (0 !=  amt_eflash_write(BOOT_FLAG_BASE + 2, &mode_value, 1)) {
                AT_BACK_ERR();
                return;
            }
        } else if (version_type == 1) { //amt mode
            mode_value = 5;

            if (0 !=  amt_eflash_write(BOOT_FLAG_BASE + 2, &mode_value, 1)) {
                AT_BACK_ERR();
                return;
            }
        } else {
            AT_BACK_ERR();
            return;
        }

        atserver_send("\r\n+versionType: %d\r\nOK\r\n", version_type);
        return;
    } else {
        LOGE(TAG,"run %s %d", cmd, type);
    }
}
#endif

#endif /* CONFIG_CHIP_ZX297100 */
