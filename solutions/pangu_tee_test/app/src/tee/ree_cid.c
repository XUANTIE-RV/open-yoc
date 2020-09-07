/*
 * Copyright (C) 2017 C-SKY Microsystems Co., Ltd. All rights reserved.
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

#include <string.h>
#include <stdint.h>
#include "tee_debug.h"
#include "tee_test.h"
#include "drv/tee.h"

#define CID_LEN  32

void tee_cid_test(void)
{
    static uint8_t cid[CID_LEN + 1] = {0};
    uint32_t len = CID_LEN + 1;
    uint8_t  cc = 1;

    while (cc--) {
        csi_tee_get_cid(cid, &len);
        TEE_LOGI("cid: %s\n", cid);
        TEE_LOGI("cid length: %d\n", len);
    }
}
