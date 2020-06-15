/*
 * Copyright (C) 2018 C-SKY Microsystems Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <stdlib.h>
#include <aos/log.h>
#include <aos/kernel.h>

#include "mdm_at.h"
#include "mdm_api.h"

#define MDM_SEMWAIT_TIMEOUT  (20 * 1000)

static const char* TAG = "mdm_api";

static aos_sem_t g_api_sem;
static char* g_atcmd;

static void handle_callback(char *at_cmd)
{
    if (g_atcmd != NULL) {
        aos_free(g_atcmd);
    }
    g_atcmd = strdup(at_cmd);
    aos_sem_signal(&g_api_sem);
}

int mdm_api_init(void)
{
    int ret;

    ret = aos_sem_new(&g_api_sem, 0);

    if (ret < 0) {
        return -1;
    }

    return 0;
}

int mdm_get_imsi(uint8_t *imsi, uint32_t len)
{
    int ret;

    if (!aos_sem_is_valid(&g_api_sem)) {
        return -1;
    }

    ret = mdm_send_from_api("AT+CIMI\r", 8, handle_callback);
    if (ret < 0) {
        LOGE(TAG, "imsi");
        return -1;
    }

    if (aos_sem_wait(&g_api_sem, MDM_SEMWAIT_TIMEOUT)) {
        LOGE(TAG, "sem wait");
        return -1;
    }

    if (g_atcmd == NULL) {
        return -1;
    }

    LOGD(TAG, "%s", g_atcmd);

    if (strstr(g_atcmd, "ERROR")) {
        aos_free(g_atcmd);
        g_atcmd = NULL;
        return -2;
    }

    if (strstr(g_atcmd, "+CIMI") == NULL) {
        aos_free(g_atcmd);
        g_atcmd = NULL;
        return -2;
    }

    strlcpy((char*)imsi, g_atcmd + 9, len);

    aos_free(g_atcmd);
    g_atcmd = NULL;

    return 15;
}

#ifndef CONFIG_IMSI
const char *aos_get_imsi(void)
{
    int ret;
    static char yoc_imsi[20];

    ret = mdm_get_imsi((uint8_t *)yoc_imsi, sizeof(yoc_imsi));

    if (ret < 0) {
        LOGE(TAG, "get imsi %d", ret);
        return NULL;
    }

    yoc_imsi[ret] = 0;

    return yoc_imsi;
}
#endif

int mdm_get_imei(uint8_t *imei, uint32_t len)
{
    int ret;

    if (!aos_sem_is_valid(&g_api_sem)) {
        return -1;
    }

    ret = mdm_send_from_api("AT+CGSN\r", 8, handle_callback);
    if (ret < 0) {
        LOGE(TAG, "imei");
        return -1;
    }

    if (aos_sem_wait(&g_api_sem, MDM_SEMWAIT_TIMEOUT)) {
        LOGE(TAG, "sem wait");
        return -1;
    }

    if (g_atcmd == NULL) {
        return -1;
    }

    LOGD(TAG, "%s", g_atcmd);

    if (strstr(g_atcmd, "ERROR")) {
        aos_free(g_atcmd);
        g_atcmd = NULL;
        return -2;
    }

    if (strstr(g_atcmd, "+CGSN") == NULL) {
        aos_free(g_atcmd);
        g_atcmd = NULL;
        return -2;
    }

    strlcpy((char*)imei, g_atcmd + 9, len);

    aos_free(g_atcmd);
    g_atcmd = NULL;

    return 15;
}

#ifndef CONFIG_IMEI
const char *aos_get_imei(void)
{
    int ret;

    static char yoc_imei[20];
    ret = mdm_get_imei((uint8_t *)yoc_imei, sizeof(yoc_imei));

    if (ret < 0) {
        LOGE(TAG, "get imei %d", ret);
        return NULL;
    }

    yoc_imei[ret] = 0;
    return yoc_imei;
}
#endif

int mdm_get_band(enum mdm_band* band)
{
    int ret;

    if (!aos_sem_is_valid(&g_api_sem)) {
        return -1;
    }

    ret = mdm_send_from_api("AT+ZBANDSET?\r", 13, handle_callback);
    if (ret < 0) {
        LOGE(TAG, "band");
        return -1;
    }

    if (aos_sem_wait(&g_api_sem, MDM_SEMWAIT_TIMEOUT)) {
        LOGE(TAG, "sem wait");
        return -1;
    }

    if (g_atcmd == NULL) {
        return -1;
    }

    LOGD(TAG, "%s", g_atcmd);

    if (strstr(g_atcmd, "ERROR")) {
        aos_free(g_atcmd);
        g_atcmd = NULL;
        return -2;
    }

    if (strstr(g_atcmd, "+ZBANDSET") == NULL) {
        aos_free(g_atcmd);
        g_atcmd = NULL;
        return -2;
    }

    if (strstr(g_atcmd, "3,5,8")) {
        *band = BAND_ALL;
        aos_free(g_atcmd);
        g_atcmd = NULL;

        return 0;
    }

    if (strchr(g_atcmd, '3')) {
        *band = BAND_3;
    }

    if (strchr(g_atcmd, '5')) {
        *band = BAND_5;
    }

    if (strchr(g_atcmd, '8')) {
        *band = BAND_8;
    }

    aos_free(g_atcmd);
    g_atcmd = NULL;

    return 0;
}

int mdm_set_band(enum mdm_band band)
{
    int ret;
    char atcmd[20];

    if (!aos_sem_is_valid(&g_api_sem)) {
        return -1;
    }

    if (band == BAND_ALL) {
        snprintf(atcmd, 20, "AT+ZBANDSET=3,5,8\r");
    } else {
        snprintf(atcmd, 20, "AT+ZBANDSET=%d\r", band);
    }

    ret = mdm_send_from_api(atcmd, strlen(atcmd), handle_callback);
    if (ret < 0) {
        LOGE(TAG, "band");
        return -1;
    }

    if (aos_sem_wait(&g_api_sem, MDM_SEMWAIT_TIMEOUT)) {
        LOGE(TAG, "sem wait");
        return -1;
    }

    if (g_atcmd == NULL) {
        return -1;
    }

    LOGD(TAG, "%s", g_atcmd);

    if (strstr(g_atcmd, "ERROR")) {
        aos_free(g_atcmd);
        g_atcmd = NULL;
        return -2;
    }

    aos_free(g_atcmd);
    g_atcmd = NULL;

    return 0;
}

int mdm_get_esq(struct mdm_esq* esq)
{
    int ret;

    if (!aos_sem_is_valid(&g_api_sem)) {
        return -1;
    }

    ret = mdm_send_from_api("AT+ZESQ\r", 8, handle_callback);
    if (ret < 0) {
        LOGE(TAG, "esq");
        return -1;
    }

    if (aos_sem_wait(&g_api_sem, MDM_SEMWAIT_TIMEOUT)) {
        LOGE(TAG, "sem wait");
        return -1;
    }

    if (g_atcmd == NULL) {
        return -1;
    }

    LOGD(TAG, "%s", g_atcmd);

    if (strstr(g_atcmd, "ERROR")) {
        aos_free(g_atcmd);
        g_atcmd = NULL;
        return -2;
    }

    int data[4], i = 0;

    char *head = g_atcmd + 8;

    while (head) {
        head += 1;
        data[i] = atoi(head);

        head = strchr(head, ',');
        i ++;

    }

    esq->rsrp = data[0];
    esq->rsrq = data[1];
    esq->rssi = data[2];
    esq->sinr = data[3];

    aos_free(g_atcmd);
    g_atcmd = NULL;

    return 0;
}

int mdm_get_psmmode(enum mdm_psm_mode* mode, struct mdm_psm_time* time)
{
    int ret;

    if (!aos_sem_is_valid(&g_api_sem)) {
        return -1;
    }

    ret = mdm_send_from_api("AT+CPSMS?\r", 10, handle_callback);
    if (ret < 0) {
        LOGE(TAG, "psm");
        return -1;
    }

    if (aos_sem_wait(&g_api_sem, MDM_SEMWAIT_TIMEOUT)) {
        LOGE(TAG, "sem wait");
        return -1;
    }

    if (g_atcmd == NULL) {
        return -1;
    }

    LOGD(TAG, "%s", g_atcmd);

    if (strstr(g_atcmd, "ERROR")) {
        aos_free(g_atcmd);
        g_atcmd = NULL;
        return -2;
    }

    *mode = atoi(g_atcmd + 9);

    strlcpy(time->requested_periodic, g_atcmd + 19, 8);
    strlcpy(time->requested_active, g_atcmd + 30, 8);
    *(time->requested_periodic + 8) = 0;
    *(time->requested_active + 8) = 0;

    aos_free(g_atcmd);
    g_atcmd = NULL;

    return 0;
}

int mdm_set_psmmode(enum mdm_psm_mode mode, struct mdm_psm_time* time)
{
    int ret;
    char atcmd[50];

    if (!aos_sem_is_valid(&g_api_sem)) {
        return -1;
    }

    snprintf(atcmd, 50, "AT+CPSMS=1,,,\"%s\",\"%s\"\r", time->requested_periodic, time->requested_active);

    ret = mdm_send_from_api(atcmd, strlen(atcmd), handle_callback);
    if (ret < 0) {
        LOGE(TAG, "psm");
        return -1;
    }

    if (aos_sem_wait(&g_api_sem, MDM_SEMWAIT_TIMEOUT)) {
        LOGE(TAG, "sem wait");
        return -1;
    }

    if (g_atcmd == NULL) {
        return -1;
    }

    LOGD(TAG, "%s", g_atcmd);

    if (strstr(g_atcmd, "ERROR")) {
        aos_free(g_atcmd);
        g_atcmd = NULL;
        return -2;
    }

    aos_free(g_atcmd);
    g_atcmd = NULL;

    return 0;
}

int mdm_get_edrxmode(enum mdm_edrx_mode *mode, struct mdm_edrx_time* time)
{
    int ret;

    if (!aos_sem_is_valid(&g_api_sem)) {
        return -1;
    }

    ret = mdm_send_from_api("AT+CEDRXS?\r", 11, handle_callback);
    if (ret < 0) {
        LOGE(TAG, "edrx");
        return -1;
    }

    if (aos_sem_wait(&g_api_sem, MDM_SEMWAIT_TIMEOUT)) {
        LOGE(TAG, "sem wait");
        return -1;
    }

    if (g_atcmd == NULL) {
        return -1;
    }

    LOGD(TAG, "%s", g_atcmd);

    if (strstr(g_atcmd, "ERROR")) {
        aos_free(g_atcmd);
        g_atcmd = NULL;
        return -2;
    }

    *mode = atoi(g_atcmd + 11);

    strlcpy(time->requested_edrx_value, g_atcmd + 14, 4);
    strlcpy(time->nw_provided_edrx_value, g_atcmd + 21, 4);
    //strlcpy(time->paging_time_window, g_atcmd + 29, 4);
    *(time->requested_edrx_value + 4) = 0;
    *(time->nw_provided_edrx_value + 4) = 0;
    //*(time->paging_time_window) = 0;

    aos_free(g_atcmd);
    g_atcmd = NULL;

    return 0;
}

int mdm_set_edrxmode(enum mdm_edrx_mode mode, struct mdm_edrx_time* time)
{
    int ret;
    char atcmd[50];

    if (!aos_sem_is_valid(&g_api_sem)) {
        return -1;
    }

    snprintf(atcmd, 50, "AT+CEDRXS=1,%d,\"%s\"\r", mode, time->requested_edrx_value);

    ret = mdm_send_from_api(atcmd, strlen(atcmd), handle_callback);
    if (ret < 0) {
        LOGE(TAG, "edrx");
        return -1;
    }

    if (aos_sem_wait(&g_api_sem, MDM_SEMWAIT_TIMEOUT)) {
        LOGE(TAG, "sem wait");
        return -1;
    }

    if (g_atcmd == NULL) {
        return -1;
    }

    LOGD(TAG, "%s", g_atcmd);

    if (strstr(g_atcmd, "ERROR")) {
        aos_free(g_atcmd);
        g_atcmd = NULL;
        return -2;
    }

    aos_free(g_atcmd);
    g_atcmd = NULL;

    return 0;
}

int mdm_get_ereg(enum mdm_ereg_mode* mode)
{
    int ret;

    if (!aos_sem_is_valid(&g_api_sem)) {
        return -1;
    }

    ret = mdm_send_from_api("AT+CEREG?\r", 10, handle_callback);
    if (ret < 0) {
        LOGE(TAG, "ereg");
        return -1;
    }

    if (aos_sem_wait(&g_api_sem, MDM_SEMWAIT_TIMEOUT)) {
        LOGE(TAG, "sem wait");
        return -1;
    }

    if (g_atcmd == NULL) {
        return -1;
    }

    LOGD(TAG, "%s", g_atcmd);

    if (strstr(g_atcmd, "ERROR")) {
        aos_free(g_atcmd);
        g_atcmd = NULL;
        return -2;
    }

    *mode = atoi(g_atcmd + 12);

    aos_free(g_atcmd);
    g_atcmd = NULL;

    return 0;
}

