#include "lscache.h"
#include "lscache_msp.h"
#include "reg_lscache.h"
#include "field_manipulate.h"
#include "compile_flag.h"

XIP_BANNED void lscache_cache_enable(uint8_t prefetch)
{
    lscache_msp_init();
    LSCACHE->CCR = FIELD_BUILD(LSCACHE_SET_PREFETCH, prefetch) | FIELD_BUILD(LSCACHE_EN, 1);
}


