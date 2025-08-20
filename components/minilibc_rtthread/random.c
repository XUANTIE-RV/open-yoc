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

#include <sys/random.h>
#include <errno.h>
#if defined(CONFIG_TEE_CA)
#include <drv/tee.h>
#endif

ssize_t getrandom(void *buf, size_t buflen, unsigned int flags)
{
    (void)flags;
    if (buf == NULL) {
        errno = EFAULT;
        return -1;
    }
#if defined(CONFIG_TEE_CA)
    csi_tee_rand_generate(buf, buflen);
#else
    int i;
    uint32_t random;
    int mod = buflen % 4;
    int count = 0;
    unsigned char *output = buf;
    static uint32_t rnd = 0x12345;
    for (i = 0; i < buflen / 4; i++) {
        random = rnd * 0xFFFF777;
        rnd = random;
        output[count++] = (random >> 24) & 0xFF;
        output[count++] = (random >> 16) & 0xFF;
        output[count++] = (random >> 8) & 0xFF;
        output[count++] = (random) & 0xFF;
    }
    random = rnd * 0xFFFF777;
    rnd = random;
    for (i = 0; i < mod; i++) {
        output[i + count] = (random >> 8 * i) & 0xFF;
    }
#endif
    return buflen;
}
