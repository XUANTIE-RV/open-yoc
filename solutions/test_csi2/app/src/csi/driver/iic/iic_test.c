/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <iic_test.h>

test_func_map_t iic_test_funcs_map[] = {
    {"IIC_INTERFACE", test_iic_interface},
    {"IIC_MASTER_ASYNC_SEND", test_iic_masterAsyncSend},
    {"IIC_MASTER_SYNC_SEND", test_iic_masterSyncSend},
    {"IIC_MASTER_DMA_SEND", test_iic_masterDmaSend},
    {"IIC_SLAVE_ASYNC_SEND", test_iic_slaveAsyncSend},
    {"IIC_SLAVE_SYNC_SEND", test_iic_slaveSyncSend},
    {"IIC_SLAVE_DMA_SEND", test_iic_slaveDmaSend},
    {"IIC_MASTER_ASYNC_RECEIVE", test_iic_masterAsyncReceive},
    {"IIC_MASTER_SYNC_RECEIVE", test_iic_masterSyncReceive},
    {"IIC_MASTER_DMA_RECEIVE", test_iic_masterDmaReceive},
    {"IIC_SLAVE_ASYNC_RECEIVE", test_iic_slaveAsyncReceive},
    {"IIC_SLAVE_SYNC_RECEIVE", test_iic_slaveSyncReceive},
    {"IIC_SLAVE_DMA_RECEIVE", test_iic_slaveDmaReceive},
    {"IIC_MEMORY_TRANSFER", test_iic_memoryTransfer},
    {"IIC_GET_STATE_MASTER_SEND", test_iic_getStateMasterSend},
    {"IIC_GET_STATE_SLAVE_RECEIVE", test_iic_getStateSlaveReceive},
};

int test_iic_main(char *args)
{
    uint8_t i;

    for (i = 0; i < sizeof(iic_test_funcs_map) / sizeof(test_func_map_t); i++) {
        if (!strcmp((void *)_mc_name, iic_test_funcs_map[i].test_func_name)) {
            (*(iic_test_funcs_map[i].test_func_p))(args);
            return 0;
        }
    }

    TEST_CASE_TIPS("IIC module don't support this command.");
    return -1;
}
