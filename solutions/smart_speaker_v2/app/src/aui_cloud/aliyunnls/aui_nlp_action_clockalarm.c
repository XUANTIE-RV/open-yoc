/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */
#include "ex_cjson.h"

#if 0
#include <sys/time.h>
#include <clock_alarm.h>

#include "aui_nlp.h"
#define TAG "nlpclock"

/**
 * 将字日期符串转换为time_t时间
 * */
static time_t time_convert(char *time)
{
    /* time format: 2020-04-25 12:00:00 */
    int       y = 0, m = 0, d = 0, hour = 0, min = 0, sec = 0;
    char *    cstart = NULL, *ntime = NULL;
    struct tm settime;
    char *    date[2] = {NULL, NULL};

    if (NULL == time) {
        LOGE(TAG, "%s: time is null pointer!", __FUNCTION__);
        return -1;
    }

    ntime         = strdup(time);
    char *saveptr = NULL;
    date[0]       = strtok_r(ntime, " ", &saveptr);
    date[1]       = strtok_r(NULL, " ", &saveptr);
    if (NULL == date[0] || NULL == date[1]) {
        LOGE(TAG, "%s: time format incorrect %s!", __FUNCTION__, time);
    }

    saveptr = NULL;
    cstart  = strtok_r(date[0], "-", &saveptr);
    if (cstart) {
        y      = atoi(cstart);
        cstart = strtok_r(NULL, "-", &saveptr);
    }
    if (cstart) {
        m      = atoi(cstart);
        cstart = strtok_r(NULL, "-", &saveptr);
    }
    if (cstart) {
        d = atoi(cstart);
    }

    saveptr = NULL;
    cstart  = strtok_r(date[1], ":", &saveptr);
    if (cstart) {
        hour   = atoi(cstart);
        cstart = strtok_r(NULL, ":", &saveptr);
    }
    if (cstart) {
        min    = atoi(cstart);
        cstart = strtok_r(NULL, ":", &saveptr);
    }
    if (cstart) {
        sec = atoi(cstart);
    }
    free(ntime);

    LOGD(TAG, "str time convert %04d/%02d/%02d %02d:%02d:%02d", y, m, d, hour, min, sec);
    memset(&settime, 0, sizeof(settime));
    settime.tm_year = y - 1900;
    settime.tm_mon  = m - 1;
    settime.tm_mday = d;
    settime.tm_hour = hour;
    settime.tm_min  = min;
    settime.tm_sec  = sec;

    time_t t_settime = mktime(&settime);
    if (t_settime > 0) {
        LOGD(TAG, "time convert to %d", t_settime - 8 * 3600);
        return t_settime - 8 * 3600;
    }

    return -1;
}

int aui_nlp_action_set_alarm(cJSON *js, int action)
{
    char *s_start_date = NULL, *s_time = NULL;
    int   clock_id = 0;

    if (action == NLP_ACTION_ALARM_ADD) {
        /* 1. parse nlp parameters */
        cJSON *action_params = cJSON_GetObjectItemByPath(js, "payload.action_params");
        cJSON *param         = NULL;
        cJSON_ArrayForEach(param, action_params)
        {
            cJSON *param_name = cJSON_GetObjectItemByPath(param, "name");
            if (cJSON_IsString(param_name) && strcmp(param_name->valuestring, "startDate") == 0) {
                cJSON *param_value = cJSON_GetObjectItemByPath(param, "value");
                if (cJSON_IsString(param_value)) {
                    s_start_date = param_value->valuestring;
                }
            }
            if (cJSON_IsString(param_name) && strcmp(param_name->valuestring, "endDate") == 0) {
                cJSON *param_value = cJSON_GetObjectItemByPath(param, "value");
                if (cJSON_IsString(param_value)) {
                    // s_end_date = param_value->valuestring;
                }
            }
            if (cJSON_IsString(param_name) && strcmp(param_name->valuestring, "time") == 0) {
                cJSON *param_value = cJSON_GetObjectItemByPath(param, "value");
                if (cJSON_IsString(param_value)) {
                    s_time = param_value->valuestring;
                }
            }
        }

        if (NULL == s_time) {
            LOGE(TAG, "clock alarm action parse err");
            return -1;
        }

        /* 2. convert time */
        time_t t_settime;
        if ((s_start_date != NULL) && (s_time != NULL)) {
            /* returned by mitasr = 0
            format: {"name":"startDate","value":"2020-04-24"}
                    {"name":"endDate","value":"2020-04-24"}
                    {"name":"time","value":"13:20:00"}
            */
            int  time_len = strlen(s_start_date) + strlen(s_time) + 1;
            char set_time[time_len + 1];
            int  ret = snprintf(set_time, time_len + 1, "%s %s", s_start_date, s_time);
            if (ret != time_len) {
                LOGE(TAG, "connect string error! date: %s, time %s ret %d time_len %d",
                     s_start_date, s_time, ret, time_len);
                return -1;
            }
            t_settime = time_convert(set_time);
        } else {
            /* returned by mitasr = 1
            format: {"name":"time","value":"2020-04-24 13:15:00"}
            */
            t_settime = time_convert(s_time);
        }
        if (t_settime < 0) {
            return -1;
        }

        /* 3. set clock alarm */
        clock_id = clock_alarm_abstime_set(clock_id, t_settime, 0);
        if (clock_id < 0) {
            return clock_id;
        }
        clock_alarm_enable(clock_id, 1);
        LOGD(TAG, "add alarm at time %s %s is set", s_start_date, s_time);
    } else { /* action =  NLP_ACTION_ALARM_DEL*/
        for (clock_id = 1; clock_id < CLOCK_ALARM_NUM + 1; clock_id++) {
            clock_alarm_set(clock_id, NULL);
        }
        LOGD(TAG, "alarm deleted");
    }

    return 0;
}
#else
int aui_nlp_action_set_alarm(cJSON *js, int action)
{
    return 0;
}
#endif
