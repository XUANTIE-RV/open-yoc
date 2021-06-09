/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include "rsa_test.h"

test_func_map_t rsa_test_funcs_map[] = {
    {"RSA_SET_KEY", test_set_rsa_key},
    {"RSA_ENCRYPT_SYNC", test_rsa_encrypt_sync},
    {"RSA_DECRYPT_SYNC", test_rsa_decrypt_sync},
    {"RSA_ENCRYPT_ASYNC", test_rsa_encrypt_async},
    {"RSA_DECRYPT_ASYNC", test_rsa_decrypt_async},
    {"RSA_ENCRYPT_DECRYPT_SYNC_PERFORMANCE", test_rsa_encrypt_decrypt_sync_performance},
    {"RSA_ENCRYPT_DECRYPT_ASYNC_PERFORMANCE", test_rsa_encrypt_decrypt_async_performance},
    {"RSA_SIGN_VERIFY_SYNC", test_rsa_sign_verify_sync},
    {"RSA_SIGN_VERIFY_ASYNC", test_rsa_sign_verify_async},
    {"RSA_SIGN_VERIFY_SYNC_PERFORMANCE", test_rsa_sign_verify_sync_performance},
    {"RSA_SIGN_VERIFY_ASYNC_PERFORMANCE", test_rsa_sign_verify_async_performance},
    {"RSA_ENCRYPT_DECRYPT_SYNC_STABILITY", rsa_encrypt_decrypt_sync_stability},
    {"RSA_ENCRYPT_DECRYPT_ASYNC_STABILITY", rsa_encrypt_decrypt_async_stability},
    {"RSA_SIGN_VERIFY_SYNC_STABILITY", rsa_sign_verify_sync_stability},
    {"RSA_SIGN_VERIFY_ASYNC_STABILITY", rsa_sign_verify_async_stability}

};

int test_rsa_main(char *args)
{
    uint8_t i;

    for (i = 0; i < sizeof(rsa_test_funcs_map) / sizeof(test_func_map_t); i++) {
        if (!strcmp((void *)_mc_name, rsa_test_funcs_map[i].test_func_name)) {
            (*(rsa_test_funcs_map[i].test_func_p))(args);
            return 0;
        }
    }

    TEST_CASE_TIPS("RSA module don't support this command.");
    return -1;
}
