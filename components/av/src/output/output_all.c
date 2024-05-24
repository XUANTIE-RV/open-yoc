/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#include "av/output/ao.h"
#include "av/output/output.h"
#include "av/output/output_all.h"

/**
 * @brief  regist audio output for dummy
 * @return 0/-1
 */
int ao_register_dummy()
{
    extern struct ao_ops ao_ops_dummy;
    return ao_ops_register(&ao_ops_dummy);
}

/**
 * @brief  regist audio output for alsa
 * @return 0/-1
 */
int ao_register_alsa()
{
    extern struct ao_ops ao_ops_alsa;
    return ao_ops_register(&ao_ops_alsa);
}

/**
 * @brief  regist audio output for alsa_standard
 * @return 0/-1
 */
int ao_register_alsa_standard()
{
    extern struct ao_ops ao_ops_alsa_standard;
    return ao_ops_register(&ao_ops_alsa_standard);
}

/**
 * @brief  regist all output
 * attention: can rewrite this function by caller
 * @return 0/-1
 */
__attribute__((weak)) int ao_register_all()
{
#if defined(CONFIG_AV_AO_DUMMY)
    REGISTER_AV_AO(DUMMY, dummy);
#endif

#if defined(CONFIG_AV_AO_ALSA)
    REGISTER_AV_AO(ALSA, alsa);
#endif

#if defined(CONFIG_AV_AO_ALSA_STANDARD)
    REGISTER_AV_AO(ALSA_STANDARD, alsa_standard);
#endif

    return 0;
}

