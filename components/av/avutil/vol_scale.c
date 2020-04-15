/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#include <math.h>
#include "avutil/vol_scale.h"

#define TAG                   "volscale"

#define FACTOR_N (256)
static int32_t _factor_scale[FACTOR_N];

static void _factor_init(int maxdb, int mindb)
{
    int i;
    double db;

    for (i = 0; i < FACTOR_N; i++) {
        db               = mindb + 1.0 * (maxdb - mindb) * i / FACTOR_N;
        _factor_scale[i] = pow(10.0, db / 20.0) * (1 << 14);
    }
}

/**
 * @brief  vol scale
 * @param  [in] in
 * @param  [in] nb_samples
 * @param  [in] out
 * @param  [in] scale index : 0~255
 * @return 0/-1
 */
int vol_scale(const int16_t *in, size_t nb_samples, int16_t *out, uint8_t scale_index)
{
    int factor;
    static int init;

    if (!init) {
        _factor_init(15, -35);
        init = 1;
    }

    factor = _factor_scale[scale_index];
    if (factor > (1 << 14)) {
        int64_t v;

        while (nb_samples--) {
            v      = (*in++ * factor) >> 14;
            v      = v < 0x7fff ? v : 0x7fff;
            v      = v > -0x7fff ? v : -0x7fff;
            *out++ = (int16_t)v;
        }
    } else {
        while (nb_samples--) {
            *out++ = (*in++ * factor) >> 14;
        }
    }

    return 0;
}

#if 0
int main(int argc, char *argv[])
{
    int index;
    int nb_samples = _test_bin_len / 2;

    if (argc < 2) {
        printf("param err\n");
        return -1;
    }

    index = atoi(argv[1]);
    vol_scale((const int16_t*)_test_bin, nb_samples, (int16_t*)_test_bin, index);

    return 0;
}
#endif


