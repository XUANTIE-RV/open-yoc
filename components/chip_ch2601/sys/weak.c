/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     weak.c
 * @brief    source file for the weak
 * @version  V1.0
 * @date     04. April 2019
 ******************************************************************************/

#include <csi_config.h>
#include <soc.h>
#include <csi_core.h>

__WEAK void soc_dcache_clean_invalid_range(unsigned long addr, uint32_t size)
{
}

__WEAK void soc_dcache_clean_invalid_all(void)
{
}

__WEAK void soc_dcache_invalid_range(unsigned long addr, uint32_t size)
{
}

__WEAK void soc_dcache_clean(void)
{
}

__WEAK void soc_icache_invalid(void)
{
}

__WEAK unsigned long soc_dma_address_remap(unsigned long addr)
{
    return addr;
}

__WEAK void *soc_phys_to_virt(unsigned long p_addr)
{
    return (void *)p_addr;
}

__WEAK unsigned long soc_virt_to_phys(void *v_addr)
{
    return (unsigned long)v_addr;
}
