#ifndef K_CACHE_H
#define K_CACHE_H

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

void k_icache_enable(void);
void k_icache_disable(void);
void k_icache_invalidate(unsigned long text, size_t size);
void k_icache_invalidate_all(void);

void k_dcache_enable(void);
void k_dcache_disable(void);
void k_dcache_clean(unsigned long buffer, size_t size);
void k_dcache_invalidate(unsigned long buffer, size_t size);
void k_dcache_clean_invalidate(unsigned long buffer, size_t size);

void k_dcache_clean_all(void);
void k_dcache_invalidate_all(void);
void k_dcache_clean_invalidate_all(void);

/* for multi-core chip */
void k_cache_scu_enable(void);
void k_cache_scu_invalidate(unsigned int cpu, unsigned int ways);

#ifdef __cplusplus
}
#endif

#endif /* K_CACHE_H */
