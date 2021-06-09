/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __AD_ICORE_CP_H__
#define __AD_ICORE_CP_H__

#include <aos/types.h>
#include <aos/aos.h>

#define AD_ICORE_IPC_SERIVCE_ID 0x10

__BEGIN_DECLS__

/**
 * @brief  init audio decoder of the cp
 * @return 0/-1
 */
int ad_icore_cp_init();

__END_DECLS__

#endif /* __AD_ICORE_CP_H__ */

