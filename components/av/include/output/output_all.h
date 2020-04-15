/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __OUTPUT_ALL_H__
#define __OUTPUT_ALL_H__

#include <aos/aos.h>

__BEGIN_DECLS__

/**
 * @brief  regist audio output for alsa
 * @return 0/-1
 */
int ao_register_alsa();

/**
 * @brief  regist audio output for ipc
 * @return 0/-1
 */
int ao_register_ipc();

__END_DECLS__

#endif /* __OUTPUT_ALL_H__ */

