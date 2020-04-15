/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __ADICORE_CP_H__
#define __ADICORE_CP_H__

#include <aos/types.h>
#include <aos/aos.h>

#define ADICORE_IPC_SERIVCE_ID 0x10

__BEGIN_DECLS__

/**
 * @brief  init audio decoder of the cp
 * @return 0/-1
 */
int adicore_cp_init();

__END_DECLS__

#endif /* __ADICORE_CP_H__ */

