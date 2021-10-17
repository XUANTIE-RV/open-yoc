/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __OUTPUT_ALL_H__
#define __OUTPUT_ALL_H__

#include <aos/aos.h>
#include "avutil/av_config.h"

__BEGIN_DECLS__

#define REGISTER_AV_AO(X, x)                                          \
    {                                                                    \
        extern int ao_register_##x();                                    \
        if (CONFIG_AV_AO_##X)                                         \
            ao_register_##x();                                           \
    }

/**
 * @brief  regist audio output for alsa
 * @return 0/-1
 */
int ao_register_alsa();

/**
 * @brief  regist all output
 * @return 0/-1
 */
static inline int ao_register_all()
{
#if defined(CONFIG_AV_AO_ALSA)
    REGISTER_AV_AO(ALSA, alsa);
#endif

    return 0;
}

__END_DECLS__

#endif /* __OUTPUT_ALL_H__ */

