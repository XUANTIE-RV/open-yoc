#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>

#include "drv/tee.h"
#include "transport_utils.h"

void transport_utils_ms_to_timeval(int timeout_ms, struct timeval *tv)
{
    tv->tv_sec = timeout_ms / 1000;
    tv->tv_usec = (timeout_ms - (tv->tv_sec * 1000)) * 1000;
}


void transport_utils_random(unsigned char *output, size_t output_len)
{
#if defined(CONFIG_TEE_CA)
    csi_tee_rand_generate(output, output_len);
#else
    int i;
    uint32_t random;
    int mod = output_len % 4;
    int count = 0;
    static uint32_t rnd = 0x12345;
    for (i = 0; i < output_len / 4; i++) {
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
}