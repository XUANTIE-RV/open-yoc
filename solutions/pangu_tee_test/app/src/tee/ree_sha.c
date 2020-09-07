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

static uint32_t data_in[] = {0x64636261, 0x68676665, 0x64636261, 0x68676665, 0x64636261, 0x68676665,
                             0x64636261, 0x68676665, 0x64636261, 0x68676665, 0x64636261, 0x68676665,
                             0x64636261, 0x68676665, 0x64636261, 0x68676665, 0x64636261, 0x68676665,
                             0x64636261, 0x68676665, 0x64636261, 0x68676665, 0x64636261, 0x68676665,
                             0x64636261, 0x68676665, 0x64636261, 0x68676665, 0x64636261, 0x68676665,
                             0x64636261, 0x68676665
                            };


static uint32_t expect_out[][16] = {
    {0x5995e2ad, 0x2cc81839, 0x4528bb9a, 0x0ccb6b87, 0xc86aa9d1},
    {0x9d96864d, 0xcf100639, 0x8846e4c6, 0x8da10a1e, 0x3c860112, 0x6cd51260, 0x51834d2f, 0x341f2485},
    {0xa320f437, 0x4e15c9a3, 0x39dbbccd, 0x12ab1f8b, 0x36fab884, 0xe4a6f7e9, 0xcadf5659},
    {0xc33b8797, 0xa74a995a, 0x100f7f41, 0xd8ea62da, 0x04327345, 0x3f3f5e75, 0x1c8f24e5, 0x7f125af8, 0xce353a99, 0x6445b2ab, 0x159974aa, 0x72288db9},
    {0x4fe77661, 0x962f33d1, 0xee7ff604, 0xfca3605b, 0x6ca350ba, 0x910573ff, 0x64d764c9, 0x02c481cc, 0x31e5bf39, 0xb48c8acc, 0x8ff671b2, 0x8ce654c7, 0x5a24ff47, 0xdbb5df5f, 0x9f01c78c, 0x53a11fac},
};

struct sha_cast_t {
    char *case_name;
    tee_sha_type_t type;
    uint8_t hash_len;
    uint8_t *expect_out;
};

struct sha_cast_t sha_case[] = {
    {"SHA1",   TEE_SHA1,   20, (uint8_t *)(expect_out[0])},
    {"SHA256", TEE_SHA256, 32, (uint8_t *)(expect_out[1])},
    {"SHA224", TEE_SHA224, 28, (uint8_t *)(expect_out[2])},
    {"SHA384", TEE_SHA384, 48, (uint8_t *)(expect_out[3])},
    {"SHA512", TEE_SHA512, 64, (uint8_t *)(expect_out[4])},
};

#define SHA_CASE_NUM (sizeof(sha_case) / sizeof(sha_case[0]))

void tee_sha_test(void)
{
    uint8_t digest[64] = {0};
    int i, ret;

    TEE_HEX_DUMP("SHA source", (uint8_t *)data_in, sizeof(data_in));

    for (i = 0; i < SHA_CASE_NUM; i++) {
        memset(digest, 0, sizeof(digest));

        TEE_LOGI("%s TEST\n", sha_case[i].case_name);
        TEE_HEX_DUMP("expect out:", sha_case[i].expect_out, sha_case[i].hash_len);

        ret = csi_tee_sha_digest((uint8_t *)data_in, sizeof(data_in), digest, sha_case[i].type);

        if (ret == 0) {
            TEE_HEX_DUMP("digest:", digest, sha_case[i].hash_len);
            TEE_LOGI("%s digest test: %s\n", sha_case[i].case_name, memcmp(digest, sha_case[i].expect_out, sha_case[i].hash_len) ? "Fail" : "Pass");
        } else {
            TEE_LOGE("%s digest test: Fail, ret 0x%x\n", sha_case[i].case_name, ret);
        }
    }
}
