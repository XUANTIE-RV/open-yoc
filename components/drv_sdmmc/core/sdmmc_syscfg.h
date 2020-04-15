#if defined(__CSKY__)
#include "csi_core.h"
#endif
#ifdef __cplusplus
extern "C" {
#endif



#if defined(__CSKY__)
static inline void dcache_writeback(void *addr, uint32_t size)
{
    csi_dcache_clean_range(addr, size);
}

static inline void dcache_invalidate(void *addr, uint32_t size)
{
    csi_dcache_invalid_range(addr, size);
}
#ifdef __cplusplus
}
#endif

#endif



