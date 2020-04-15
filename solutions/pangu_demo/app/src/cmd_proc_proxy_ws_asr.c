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

#define TAG "app-proxy-ws-asr"

#define ASR_NAME "PWSASR"

int aui_nlp_process_proxy_ws_asr(cJSON *js, const char *json_text)
{
    LOGD(TAG, "Enter %s", __FUNCTION__);

    cJSON *identify  = cJSON_GetObjectItem(js, "identify");
    cJSON *code  = cJSON_GetObjectItem(js, "code");
    cJSON *data  = cJSON_GetObjectItem(js, "data");
    if (!(identify && code && cJSON_IsString(code))) {
        LOGD(TAG, "results code error");
        return -1;
    }

    if (atoi(code->valuestring) != 0) {
        LOGD(TAG, "code = %s", code->valuestring);
        return -1;
    }

    if (!(identify && identify->valuestring && strcmp(identify->valuestring, ASR_NAME) == 0)) {
        LOGD(TAG, "identify is not %s", ASR_NAME);
        return -1;
    }   

    if (!(data && data->valuestring)) {
        LOGD(TAG, "results data error");
        return -1;
    }

    LOGI(TAG, "parse success, buf = %s\n", data->valuestring);

    return aui_cloud_push_text(&g_aui_handler, data->valuestring);
}

