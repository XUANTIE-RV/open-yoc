/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <csi_core.h>

void k_icache_enable()
{
    csi_icache_enable();
}

void k_icache_disable()
{
    csi_icache_disable();
}

void k_icache_invalidate(unsigned long text, size_t size)
{
    //TODO:
    csi_icache_invalid();
}

void k_icache_invalidate_all()
{
    csi_icache_invalid();
}

void k_dcache_enable()
{
    csi_dcache_enable();
}

void k_dcache_disable()
{
    csi_dcache_disable();
}

void k_dcache_clean(unsigned long buffer, size_t size)
{
    csi_dcache_clean_range((uint64_t*)buffer, size);
}

void k_dcache_invalidate(unsigned long buffer, size_t size)
{
    csi_dcache_invalid_range((uint64_t*)buffer, size);
}

void k_dcache_clean_invalidate(unsigned long buffer, size_t size)
{
    csi_dcache_clean_invalid_range((uint64_t*)buffer, size);
}

