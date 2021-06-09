/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef __RSA_TEST__
#define __RSA_TEST__

#include <stdint.h>
#include <soc.h>
#include <string.h>
#include <stdlib.h>
#include <autotest.h>
#include <test_log.h>
#include <test_config.h>
#include <test_common.h>
#include <drv/rsa.h>

#define RSA_TEST_DATA_LEN  (1024)
#define RSA_KEY_LEN 2048
#define RSA_SOURCE_DATA_LEN (RSA_TEST_DATA_LEN)
#define RSA_TARGET_DATA_LEN (RSA_TEST_DATA_LEN)
#define RSA_RECV_HOST_DATA_LEN (RSA_SOURCE_DATA_LEN * 2)
#define RSA_TEST_KEY_LEN (RSA_TEST_DATA_LEN)

typedef struct {
    uint32_t    idx;
    csi_rsa_key_bits_t    key_bits;
	csi_rsa_padding_type_t    padding_type;
    csi_rsa_hash_type_t    hash_type;
    uint16_t data_len;
    uint8_t flag_gen_key;
} test_rsa_args_t;

extern uint8_t rsa_source_data[RSA_SOURCE_DATA_LEN];
extern uint8_t rsa_target_data[RSA_TARGET_DATA_LEN];
extern char rsa_received_data_from_host[RSA_RECV_HOST_DATA_LEN];
extern uint8_t rsa_test_key_n[RSA_TEST_KEY_LEN];
extern uint8_t rsa_test_key_e[RSA_TEST_KEY_LEN];
extern uint8_t rsa_test_key_d[RSA_TEST_KEY_LEN];

extern int test_rsa_encrypt_sync(char *args);
extern int test_rsa_decrypt_sync(char *args);

extern int test_rsa_encrypt_async(char *args);
extern int test_rsa_decrypt_async(char *args);

extern int test_rsa_encrypt_decrypt_sync_performance(char *args);
extern int test_rsa_encrypt_decrypt_async_performance(char *args);

extern int test_rsa_sign_verify_sync(char *args);
extern int test_rsa_sign_verify_async(char *args);

extern int test_rsa_sign_verify_sync_performance(char *args);
extern int test_rsa_sign_verify_async_performance(char *args);
extern int test_set_rsa_key(char *args);

extern int rsa_encrypt_decrypt_sync_stability(char *args);
extern int rsa_encrypt_decrypt_async_stability(char *args);
extern int rsa_sign_verify_sync_stability(char *args);
extern int rsa_sign_verify_async_stability(char *args);
#endif
