/**
 * @file aie_mem.h
 * @author Liang Shui (Gao Hao)
 * @brief 部署平台内存池api
 * @date 2023-04-25
 * 
 * Copyright (C) 2022 Alibaba Group Holding Limited
 * 
 */

#ifndef _AIE_MEM_POOL_H_
#define _AIE_MEM_POOL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

typedef enum {
     MEM_INIT_SUCCESEE = 0,
     MEM_NO_INI = 1,
     MEM_DOUBLE_INIT = 2,
} mem_pool_res;

typedef struct {
    int mem_pool_nums;
    int *mem_pool_limit;
    int *mem_pool_size;
} mem_pool_paprams_t;

// 全局仅初始化一次, 如果未初始化则直接使用系统malloc
mem_pool_res aie_global_mem_pool_init(mem_pool_paprams_t params);

#ifdef __cplusplus
}
#endif

#endif // _AIE_MEM_POOL_H_
