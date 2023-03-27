/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

/*
 * 为了统一单核和多核解决方案依赖组件，该组件再两种模式下都进行依赖
 * 双核模式，主CPU执行固件加载部分功能，节点代码不执行；算法核执行空函数，执行节点代码
 * 单核模式，算法核执行空函数，执行节点代码
 */

/* 固件加载方式 */
#include <board.h>

#if defined(CONFIG_BOARD_AMP_LOAD_FW) && CONFIG_BOARD_AMP_LOAD_FW
#include <board.h>
#include <yoc/partition.h>
#endif

int alg_kws_init(void)
{
#if defined(CONFIG_BOARD_AMP_LOAD_FW) && CONFIG_BOARD_AMP_LOAD_FW
    partition_t partition;
    unsigned long run_address;
    const char *name = "prim";

    partition = partition_open(name);
    if (partition_split_and_get(partition, 1, NULL, NULL, &run_address)) {
        printf("get [%s] run_address failed.\n", name);
        return -1;
    }
    partition_close(partition);
    return board_load_amp_fw(0, (void*)run_address, NULL, 0);
# else
    return -1;
#endif
}
