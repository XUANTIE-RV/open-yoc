/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __MCA_ALL_H__
#define __MCA_ALL_H__

#include <aos/aos.h>

__BEGIN_DECLS__

/**
 * @brief  regist mca for local
 * @return 0/-1
 */
int mcax_register_local();

/**
 * @brief  regist mca for ipc
 * @return 0/-1
 */
int mcax_register_ipc();

__END_DECLS__

#endif /* __MCA_ALL_H__ */

