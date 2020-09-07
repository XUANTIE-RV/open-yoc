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
#include "drv/tee.h"
#include "tee_debug.h"
#include "tee_test.h"

static uint8_t expect_out[][16] = {
    {0x03, 0x64, 0x0f, 0x7a, 0x01, 0xef, 0xd6, 0xa7, 0x73, 0x48, 0x35, 0x5b, 0xbf, 0x5f, 0xa9, 0xfe },
    {0x03, 0x64, 0x0f, 0x7a, 0x01, 0xef, 0xd6, 0xa7, 0x73, 0x48, 0x35, 0x5b, 0xbf, 0x5f, 0xa9, 0xfe },
    {0xdb, 0xc0, 0x1d, 0xb9, 0x2d, 0xdc, 0x2c, 0xf2, 0x4b, 0x3a, 0x79, 0x27, 0x89, 0x7f, 0xd5, 0x71 },
    {0xdb, 0xc0, 0x1d, 0xb9, 0x2d, 0xdc, 0x2c, 0xf2, 0x4b, 0x3a, 0x79, 0x27, 0x89, 0x7f, 0xd5, 0x71 },
    {0x7b, 0xab, 0x3a, 0xb2, 0xf3, 0x3c, 0xcf, 0xca, 0x20, 0x7a, 0xc9, 0x63, 0x7f, 0x02, 0x49, 0x55 },
    {0x7b, 0xab, 0x3a, 0xb2, 0xf3, 0x3c, 0xcf, 0xca, 0x20, 0x7a, 0xc9, 0x63, 0x7f, 0x02, 0x49, 0x55 }
};

struct aes_cast_t {
    char *case_name;
    tee_aes_mode_e mode;
    uint32_t key_len;
    uint8_t *expect_out;
};

struct aes_cast_t aes_case[] = {
    {"AES128 CBC", TEE_AES_MODE_CBC, 16, expect_out[0]},
    {"AES128 ECB", TEE_AES_MODE_ECB, 16, expect_out[1]},
    {"AES192 CBC", TEE_AES_MODE_CBC, 24, expect_out[2]},
    {"AES192 ECB", TEE_AES_MODE_ECB, 24, expect_out[3]},
    {"AES256 CBC", TEE_AES_MODE_CBC, 32, expect_out[4]},
    {"AES256 ECB", TEE_AES_MODE_ECB, 32, expect_out[5]}
};

#define AES_CASE_NUM (sizeof(aes_case) / sizeof(aes_case[0]))

void tee_aes_test(void)
{
    const uint8_t in[16] = "Hello, World!";
    uint8_t iv[16] = {0};
    const uint8_t key[32] = "Demo-Key";

    int32_t ret;
    uint8_t out[16] = {};
    uint8_t out2[16] = {};
    int i;

    TEE_LOGI("AES CBC source string:%s\n", in);
    TEE_HEX_DUMP("AES CBC IV:", iv, 16);
    TEE_HEX_DUMP("AES CBC KEY:", key, 32);

    for (i = 0; i < AES_CASE_NUM; i++) {
        memset(out, 0, 16);
        memset(out2, 0, 16);
        memset(iv, 0, 16);

        TEE_LOGI("%s TEST\n", aes_case[i].case_name);
        TEE_HEX_DUMP("expect out:", aes_case[i].expect_out, 16);

        ret = csi_tee_aes_encrypt(in, 16, key, aes_case[i].key_len, iv, out, aes_case[i].mode);

        if (0 == ret) {
            TEE_HEX_DUMP("encrypt:", out, 16);
            TEE_LOGI("%s encrypt test: %s\n", aes_case[i].case_name, memcmp(out, aes_case[i].expect_out, 16) ? "Fail" : "Pass");
        } else {
            TEE_LOGE("%s test: Fail, ret 0x%x\n",  aes_case[i].case_name, ret);
        }

        memset(iv, 0, 16);

        ret = csi_tee_aes_decrypt(out, 16, key, aes_case[i].key_len, iv, out2, aes_case[i].mode);

        if (0 == ret) {
            TEE_HEX_DUMP("decrypt:", out2, 16);
            TEE_LOGI("%s decrypt test: %s\n", aes_case[i].case_name, memcmp(out2, in, 16) ? "Fail" : "Pass");
        } else {
            TEE_LOGE("%s test: Fail, ret 0x%x\n", aes_case[i].case_name, ret);
        }
    }
}


