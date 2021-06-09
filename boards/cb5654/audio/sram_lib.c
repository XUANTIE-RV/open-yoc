/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <csi_core.h>
#include "silan_voice_adc.h"

extern uint32_t g_sram_clock;

SRAM_TEXT static void _10udelay(void)
{
    uint32_t load  = CORET->LOAD;
    uint32_t start = CORET->VAL;
    uint32_t cnt   = (g_sram_clock / 1000 / 100);

    while (1) {
        uint32_t cur = CORET->VAL;

        if (start > cur) {
            if (start - cur >= cnt) {
                return;
            }
        } else {
            if (load - cur + start > cnt) {
                return;
            }
        }
    }
}

SRAM_TEXT void sram_udelay(uint32_t us)
{
    us = (us / 10);

    while (us--) {
        _10udelay();
    }
}

SRAM_TEXT void *sram_memcpy(void *s1, const void *s2, size_t n)
{
    register char *r1 = s1;
    register const char *r2 = s2;

    while (n) {
        *r1++ = *r2++;
        --n;
    }

    return s1;
}

SRAM_TEXT void *sram_memmove(void *s1, const void *s2, size_t n)
{
    register char *s = (char *)s1;
    register const char *p = (const char *)s2;

    if (p >= s) {
        while (n) {
            *s++ = *p++;
            --n;
        }
    } else {
        while (n) {
            --n;
            s[n] = p[n];
        }
    }

    return s1;
}

SRAM_TEXT void *sram_memset(void *s, int c, size_t n)
{
    int i;
    uint8_t *p = (uint8_t *)s;

    for (i = 0; i < n; i++) {
        p[i] = c;
    }

    return s;
}

SRAM_TEXT int sram_memcmp(const void *s1, const void *s2, size_t n)
{
        register const uint8_t *r1 = (const uint8_t *)s1;
        register const uint8_t *r2 = (const uint8_t *)s2;

        while (n && (*r1 == *r2)) {
                ++r1;
                ++r2;
                --n;
        }

        return (n == 0) ? 0 : ((*r1 < *r2) ? -1 : 1);
}

#if SRAM_TIMER_EN
#include "silan_timer_regs.h"
/* 获取时间，单位微秒 */
SRAM_TEXT uint32_t sram_timer_get_us(void)
{
    uint32_t val = __sREG32(TIMER_ADDR(0), TIMER_COUNT);

    return (val / 6);
}
#endif
