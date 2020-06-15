/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#ifdef CONFIG_FOTA

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <aos/log.h>
#include <fota_host_api.h>
#include "at_internal.h"

#include <crc16.h>

#define TAG     "at_ota"
#define CMD_SUPPORT_NUM         2

static char *aCmd[CMD_SUPPORT_NUM] = {"FOTA INFO", "FOTA_PACKET"};


void at_cmd_fotastart(char *cmd, int type, char *data)
{
    if (type == EXECUTE_CMD) {
        int ret;
        ret = fota_host_start();

        if (0 != ret) {
            AT_BACK_RET_ERR("+FOTASTART", ret);
            return;
        }
        AT_BACK_OK();
    } else {
        LOGE(TAG,"run %s %d", cmd, type);
    }
}

void at_cmd_fotagetfull(char *cmd, int type, char *data)
{
    if (type == WRITE_CMD) {
        int ret;
        char *pTmp = (char *)data;
        char *aStr[2];
        aStr[0] = strtok(pTmp, ",");
        aStr[1] = strtok(NULL, ",");

        if ((aStr[0] != NULL) &&
                (aStr[1] != NULL)) {
            AT_LOGD(TAG, "full %s,%s", aStr[0], aStr[1]);
            ret = fota_host_get_full(atoi(aStr[0]), atoi(aStr[1]));

            if (0 != ret) {
                AT_BACK_RET_ERR("+FOTAGETFULL", FOTA_ERR_STATUS);
            }
        } else {
            AT_BACK_RET_ERR("+FOTAGETFULL", FOTA_ERR_FORMAT);
        }
    } else {
        LOGE(TAG,"run %s %d", cmd, type);
    }
}

void at_cmd_fotastop(char *cmd, int type, char *data)
{
    if (type == WRITE_CMD) {
        int res;
        char *tmp = (char *)data;
        res = atoi(tmp);
        printf("get stop res=%d\n",res);
        at_uart_send_str("+FOTASTOP:%d\r\nOK\r\n",res);
        fota_host_over(res);
    } else {
        LOGE(TAG,"run %s %d", cmd, type);
    }
}

void at_cmd_fotagetdiff(char *cmd, int type, char *data)
{
    if (type == WRITE_CMD) {
        int ret;
        char *pTmp = (char *)data;
        char *aStr[2];
        aStr[0] = strtok(pTmp, ",");
        aStr[1] = strtok(NULL, ",");

        if ((aStr[0] != NULL) &&
                (aStr[1] != NULL)) {
            AT_LOGD(TAG, "diff %s,%s", aStr[0], aStr[1]);
            ret = fota_host_get_diff(atoi(aStr[0]), atoi(aStr[1]));

            if (0 != ret) {
                AT_BACK_RET_ERR("+FOTAGETDIFF", FOTA_ERR_STATUS);
            }
        } else {
            AT_BACK_RET_ERR("+FOTAGETDIFF", FOTA_ERR_FORMAT);
        }
    } else {
        LOGE(TAG,"run %s %d", cmd, type);
    }
}

void at_fota_process_cmd(at_event_t *evt)
{
    uint16_t len;
    uint32_t offset;
    uint8_t *buf;
    uint16_t crc = 0;
    if (evt->evt_code == 0) {
        return;
    }

    switch (evt->evt_code) {
    case FOTA_AT_INFO_ID:
        if (evt->argv) {
            st_fota_info *stInfo = evt->argv;

            if (evt->evt_value) {
                fota_host_postsem();
            }

            at_uart_send_str("+FOTASTART:%s,%d,%d\r\nOK\r\n", stInfo->pVer, stInfo->size, stInfo->type);
        } else {
            AT_LOGE(TAG, "argv");
        }

        break;

    case FOTA_AT_PACKET_ID:
        if (evt->argv) {
            st_fota_package *stPack = evt->argv;
            crc = crc16(0,stPack->pDat,stPack->len);
            AT_LOGD(TAG, "get host full date\n");
            at_uart_send_str("+FOTAGETFULL:%d,%d,%d,",stPack->offset,stPack->len,crc);
            at_uart_send_data(stPack->pDat, stPack->len);
            at_uart_send_str("\r\nOK\r\n");
        } else {
            AT_LOGE(TAG, "argv");
        }

        break;

    case FOTA_AT_EVENT_ID:
        if (evt->evt_value) {
            atserver_lock();
            at_uart_send_str("+EVENT=%s\r\n", (char *)evt->evt_value);
            atserver_unlock();
        }
        fota_host_postsem();
        break;

    case FOTA_AT_FAILED_ID:
        break;

    case FOTA_AT_DIFF_ID:
        if (evt->evt_value) {
            buf = (uint8_t *)evt->evt_value;
            offset =(buf[0]<<24UL)+(buf[1] << 16UL)+(buf[2] << 8UL) + buf[3];
            len = (buf[4] << 8UL) + buf[5];
            crc = crc16(0,&buf[6],len);
            at_uart_send_str("+FOTAGETDIFF:%d,%d,%d,",offset,len,crc);
            at_uart_send_data(&buf[6], len);
            at_uart_send_str("\r\nOK\r\n");
        }
        else {
            AT_LOGE(TAG, "diff len");
        }
        fota_host_postsem();
        break;

    case FOTA_AT_ERR_ID:
        if (evt->argv) {
            st_fota_err *stErr = evt->argv;

            if (stErr->retCmdId == 0) {
                AT_BACK_ERR();
            } else {
                if (stErr->retCmdId < CMD_SUPPORT_NUM) {
                    AT_BACK_RET_ERR(aCmd[stErr->retCmdId], stErr->errNo);
                }
            }

            fota_host_postsem();
        } else {
            AT_LOGE(TAG, "argv");
        }

        break;

    default:
        break;
    }
}
#endif
