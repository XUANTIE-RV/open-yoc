 /*
 * Copyright (C) 2017-2024 Alibaba Group Holding Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <aos/debug.h>
#include <yoc/nvram.h>
#include <drv/tee.h>

#define PRODUCTKEY "PRODUCTKEY"
#define DEVICENAME "DEVICENAME"
#define DEVICESECRET "DEVICESECRET"
#define TAG "YIO"

#define KEY_BUFFER_SIZE 48
#define NAME_BUFFER_SIZE 108

int nvram_get_iot_info(char *product_key, uint32_t *product_key_len, char *device_name,
                       uint32_t *device_name_len, char *device_secret,
                       uint32_t *device_secret_len)
{
    int ret = -1;
    if (product_key == NULL || product_key_len == NULL || device_name == NULL ||
        device_name_len == NULL || device_secret == NULL || device_secret_len == NULL) {
        return -EINVAL;
    }

    /* get alicoap device_info from factory setting area */
    ret = nvram_get_val(PRODUCTKEY, product_key, *product_key_len);
    if (ret <= 0) {
        printf("PRODUCTKEY");
        goto do_error;
    }
    product_key[ret] = '\0';

    ret = nvram_get_val(DEVICENAME, device_name, *device_name_len);
    if (ret <= 0) {
        printf("DEVICENAME");
        goto do_error;
    }
    device_name[ret] = '\0';

    ret = nvram_get_val(DEVICESECRET, device_secret, *device_secret_len);
    if (ret <= 0) {
        printf("DEVICESECRET");
        goto do_error;
    }
    device_secret[ret] = '\0';

do_error:
    return ret <= 0 ? -1 : 0;
}
