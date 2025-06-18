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
#include <aos/aos.h>
#include "dsp/csi_math.h"

#define DSP_THREAD_NUM (5)
static volatile int g_dsp_cnt;
static aos_task_t g_dsp_handles[DSP_THREAD_NUM];
static volatile int g_dsp_ret[DSP_THREAD_NUM];

#define LOOP_TIMES         (100)
#define F32_ABS_SIZE       (128)

static uint32_t f32_data0[F32_ABS_SIZE] = {
    0x4212d6bf, 0x421f940f, 0xc12037b0, 0xc08278d4, 0x41766b79, 0xc199fcff, 0x41f5517d, 0x4153dd16,
    0xc16b876f, 0x41b2486e, 0x41078229, 0x41b7e1ed, 0xc233e164, 0x41257171, 0x3fb16fd4, 0x4229bccc,
    0xc227286c, 0xc0845959, 0x4227a368, 0xc055cbfc, 0x41b56e21, 0x40f8779e, 0xc13be122, 0xc1865eef,
    0x3ffe8c74, 0xc1563306, 0xc1b7244d, 0x4224d39b, 0xc0cb174f, 0x412607a3, 0xc102aef5, 0x41090b5a,
    0xc1822a6b, 0x419e646d, 0xc190c7e9, 0x413b81db, 0x423d139a, 0x416ad60b, 0x3fee5836, 0x41841d0f,
    0xc1d568e4, 0x41f2f600, 0x423b0b8c, 0x42390893, 0xc233cb0d, 0xc11021f0, 0xc0220420, 0x4238d861,
    0xc18db68d, 0x413d5a89, 0x41c39e81, 0x420a6100, 0xc10817c8, 0x42408e99, 0xc1d8f915, 0xc1ca88d2,
    0x40e7047a, 0x4069c67c, 0x41fc28ac, 0x4239eaae, 0x420d9045, 0xc183f493, 0xc201e476, 0xc1c22ad3,
    0x4214a4a5, 0x4100f309, 0x411230fa, 0x41efae79, 0xc180fd26, 0xc1c62483, 0x41ca9bdf, 0xc21db240,
    0xc2301609, 0x41fba7d4, 0xc2409cc2, 0xc1b3a8b0, 0xc1b0a08c, 0x4113b904, 0xc1c24348, 0x412f04b6,
    0x3fc06a34, 0x4194615d, 0x4034f212, 0x42114709, 0x408ed9c6, 0x420004b9, 0xc246759c, 0xc1290edf,
    0xc1b550e5, 0x41b4f6c5, 0xc20ca82c, 0x423f0f35, 0xc14b8ac0, 0xc1f55ab2, 0xc208aa00, 0xc1a6fc01,
    0x41a7a287, 0x421b2fba, 0x41d184de, 0xc1480129, 0xc19e2f21, 0xc1765670, 0x4224cb3b, 0xc1fe8252,
    0x423a6852, 0x41f3e991, 0xc238e70b, 0x423ed793, 0x422d23e0, 0xc2255f6d, 0xc1b8e30e, 0xc148555a,
    0x4083436b, 0x4113df14, 0xc216223a, 0xc19ccb23, 0x41379d5f, 0x41cf7971, 0xc2149343, 0x41763cac,
    0x423dd8a2, 0xc2185bb6, 0x410e5a5c, 0x4184b215, 0xc1906cca, 0xc1c44eaa, 0xc231c6b2, 0x416bdef6,
};

static uint32_t f32_abs_result0[F32_ABS_SIZE] = {
    0x4212d6bf, 0x421f940f, 0x412037b0, 0x408278d4, 0x41766b79, 0x4199fcff, 0x41f5517d, 0x4153dd16,
    0x416b876f, 0x41b2486e, 0x41078229, 0x41b7e1ed, 0x4233e164, 0x41257171, 0x3fb16fd4, 0x4229bccc,
    0x4227286c, 0x40845959, 0x4227a368, 0x4055cbfc, 0x41b56e21, 0x40f8779e, 0x413be122, 0x41865eef,
    0x3ffe8c74, 0x41563306, 0x41b7244d, 0x4224d39b, 0x40cb174f, 0x412607a3, 0x4102aef5, 0x41090b5a,
    0x41822a6b, 0x419e646d, 0x4190c7e9, 0x413b81db, 0x423d139a, 0x416ad60b, 0x3fee5836, 0x41841d0f,
    0x41d568e4, 0x41f2f600, 0x423b0b8c, 0x42390893, 0x4233cb0d, 0x411021f0, 0x40220420, 0x4238d861,
    0x418db68d, 0x413d5a89, 0x41c39e81, 0x420a6100, 0x410817c8, 0x42408e99, 0x41d8f915, 0x41ca88d2,
    0x40e7047a, 0x4069c67c, 0x41fc28ac, 0x4239eaae, 0x420d9045, 0x4183f493, 0x4201e476, 0x41c22ad3,
    0x4214a4a5, 0x4100f309, 0x411230fa, 0x41efae79, 0x4180fd26, 0x41c62483, 0x41ca9bdf, 0x421db240,
    0x42301609, 0x41fba7d4, 0x42409cc2, 0x41b3a8b0, 0x41b0a08c, 0x4113b904, 0x41c24348, 0x412f04b6,
    0x3fc06a34, 0x4194615d, 0x4034f212, 0x42114709, 0x408ed9c6, 0x420004b9, 0x4246759c, 0x41290edf,
    0x41b550e5, 0x41b4f6c5, 0x420ca82c, 0x423f0f35, 0x414b8ac0, 0x41f55ab2, 0x4208aa00, 0x41a6fc01,
    0x41a7a287, 0x421b2fba, 0x41d184de, 0x41480129, 0x419e2f21, 0x41765670, 0x4224cb3b, 0x41fe8252,
    0x423a6852, 0x41f3e991, 0x4238e70b, 0x423ed793, 0x422d23e0, 0x42255f6d, 0x41b8e30e, 0x4148555a,
    0x4083436b, 0x4113df14, 0x4216223a, 0x419ccb23, 0x41379d5f, 0x41cf7971, 0x42149343, 0x41763cac,
    0x423dd8a2, 0x42185bb6, 0x410e5a5c, 0x4184b215, 0x41906cca, 0x41c44eaa, 0x4231c6b2, 0x416bdef6,
};

int dsp_main()
{
    int i, j;
    uint32_t size;
    float32_t *src;
    float32_t *dst;
    float32_t *result;
    float32_t tmp[F32_ABS_SIZE];

    src    = (float32_t *)f32_data0;
    dst    = tmp;
    result = (float32_t *)f32_abs_result0;
    size   = F32_ABS_SIZE;

    for (j = 0; j < LOOP_TIMES; j++) {
        /* for dsp context save/restore test */
        aos_msleep(10);
        memset(tmp, 0, sizeof(tmp));
        csi_abs_f32(src, dst, size);
        aos_msleep(20);
        for (i = 0; i < size; i++) {
            if (abs(dst[i] - result[i]) > 0.000001) {
                return -1;
            }
        }
    }

    return 0;
}

static void dsp_thread(void *arg)
{
    g_dsp_ret[g_dsp_cnt] = dsp_main();
    g_dsp_cnt++;
}

int example_core_dsp()
{
    int rc;

    for (int i = 0; i < DSP_THREAD_NUM; i++) {
        rc = aos_task_new_ext(&g_dsp_handles[i], "app_task", dsp_thread,
                              NULL, 4*1024, AOS_DEFAULT_APP_PRI);
        if (rc) {
            printf("may be oom! rc = %d\n", rc);
            goto error;
        }
    }

    while (g_dsp_cnt < DSP_THREAD_NUM) {
        aos_msleep(50);
    }
    for (int i = 0; i < DSP_THREAD_NUM; i++) {
        if (g_dsp_ret[i])
            goto error;
    }
    printf("dsp runs successfully!\n");
    return 0;

error:
    printf("dsp runs fail!\n");
    return -1;
}


