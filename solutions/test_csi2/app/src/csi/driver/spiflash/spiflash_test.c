/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <spiflash_test.h>


test_func_map_t spiflash_test_funcs_map[] = {
    {"SPIFLASH_INTERFACE", test_spiflash_interface},
    {"SPIFLASH_PROGRAM_READ", test_spiflash_program_read},
    {"SPIFLASH_QSPI_PROGRAMREAD", test_spiflash_qspi_programread},
    {"SPIFLASH_LOCK", test_spiflash_lock},
};


int test_spiflash_main(char *args)
{
    uint8_t i;

    for (i = 0; i < sizeof(spiflash_test_funcs_map) / sizeof(test_func_map_t); i++) {
        if (!strcmp((void *)_mc_name, spiflash_test_funcs_map[i].test_func_name)) {
            (*(spiflash_test_funcs_map[i].test_func_p))(args);
            return 0;
        }
    }

    TEST_CASE_TIPS("SPIFLASH module don't support this command.");
    return -1;
}
