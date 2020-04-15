/*
 * Copyright (C) 2019 C-SKY Microsystems Co., Ltd. All rights reserved.
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

#include <yoc_config.h>
#include <stdlib.h>
#include <string.h>
#include <aos/aos.h>

#include "app_main.h"

#define TAG "app"

//#define WORD_PREFIX ("\\\"w\\\":\\\"")
#define WORD_PREFIX ("\"w\":\"")

int aui_nlp_process_xf_rtasr(cJSON *js, const char *json_text)
{
    int len = 0;
    char *next;
    char buf[128] = {0};
    LOGD(TAG, "Enter %s", __FUNCTION__);

    cJSON *code  = cJSON_GetObjectItem(js, "code");
    cJSON *data  = cJSON_GetObjectItem(js, "data");
    if (!(code && cJSON_IsString(code))) {
        LOGD(TAG, "results code error");
        return -1;
    }

    if (atoi(code->valuestring) != 0) {
        LOGD(TAG, "code = %s", code->valuestring);
        return -1;
    }

    if (!(data && data->valuestring)) {
        LOGD(TAG, "results data error");
        return -1;
    }

    next = strstr(data->valuestring, WORD_PREFIX);
    while (next) {
        next += strlen(WORD_PREFIX);
        //printf("next = %s\n", next);
        while ((*next != '"') && (len < sizeof(buf) - 1)) {
            buf[len++] = *next;
            next++;
        }
        next = strstr(next, WORD_PREFIX);
    }
    LOGI(TAG, "parse success, buf = %s, len = %d\n", buf, len);

    return len ? aui_cloud_push_text(&g_aui_handler, buf) : -1;
}

