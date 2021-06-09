/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include "aes_test.h"

test_func_map_t aes_test_funcs_map[] = {
    {"SET_AES_KEY", set_aes_key},
    {"SET_AES_IV", set_aes_iv},
    {"AES_ECB_ENCRYPT", aes_ecb_encrypt},
    {"AES_ECB_DECRYPT", aes_ecb_decrypt},
    {"AES_CBC_ENCRYPT", aes_cbc_encrypt},
    {"AES_CBC_DECRYPT", aes_cbc_decrypt},
    {"AES_CFB128_ENCRYPT", aes_cfb128_encrypt},
    {"AES_CFB128_DECRYPT", aes_cfb128_decrypt},
    {"AES_CTR_ENCRYPT", aes_ctr_encrypt},
    {"AES_CTR_DECRYPT", aes_ctr_decrypt},
    {"AES_ECB_ENCRYPT_PERFORMANCE", aes_ecb_encrypt_performance},
    {"AES_ECB_DECRYPT_PERFORMANCE", aes_ecb_decrypt_performance},
    {"AES_CBC_ENCRYPT_PERFORMANCE", aes_cbc_encrypt_performance},
    {"AES_CBC_DECRYPT_PERFORMANCE", aes_cbc_decrypt_performance},
    {"AES_ECB_ENCRYPT_DECRYPT_STABILITY", aes_ecb_encrypt_decrypt_stability},
    {"AES_CBC_ENCRYPT_DECRYPT_STABILITY", aes_cbc_encrypt_decrypt_stability},
};

int test_aes_main(char *args)
{
    uint8_t i;

    for (i = 0; i < sizeof(aes_test_funcs_map) / sizeof(test_func_map_t); i++) {
        if (!strcmp((void *)_mc_name, aes_test_funcs_map[i].test_func_name)) {
            (*(aes_test_funcs_map[i].test_func_p))(args);
            return 0;
        }
    }

    TEST_CASE_TIPS("AES module don't support this command.");
    return -1;
}
