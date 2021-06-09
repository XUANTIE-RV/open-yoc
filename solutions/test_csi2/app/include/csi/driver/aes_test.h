/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef __AES_TEST__
#define __AES_TEST__

#include <stdint.h>
#include <soc.h>
#include <string.h>
#include <stdlib.h>
#include <autotest.h>
#include <test_log.h>
#include <test_config.h>
#include <test_common.h>
#include <drv/aes.h>

#define AES_DATA_LEN	128
#define AES_KEY_LEN	64
#define AES_IV_LEN	32

typedef struct {
    uint32_t    idx;
    uint32_t    mode;
    uint32_t    key;
    uint32_t    key_len_bits;   //参考csi_aes_key_bits_t
}test_aes_args_t;


extern int test_aes_mode(char *args);
extern int set_aes_key(char *args);
extern int set_aes_iv(char *args);
extern int aes_ecb_encrypt(char *args);
extern int aes_ecb_decrypt(char *args);
extern int aes_cbc_encrypt(char *args);
extern int aes_cbc_decrypt(char *args);
extern int aes_cfb128_encrypt(char *args);
extern int aes_cfb128_decrypt(char *args);
extern int aes_ctr_encrypt(char *args);
extern int aes_ctr_decrypt(char *args);
extern int aes_ecb_encrypt_performance(char *args);
extern int aes_ecb_decrypt_performance(char *args);
extern int aes_cbc_encrypt_performance(char *args);
extern int aes_cbc_decrypt_performance(char *args);
extern int aes_cfb128_encrypt_performance(char *args);
extern int aes_cfb128_decrypt_performance(char *args);
extern int aes_ctr_encrypt_performance(char *args);
extern int aes_ctr_decrypt_performance(char *args);
extern int aes_ecb_encrypt_decrypt_stability(char *args);
extern int aes_cbc_encrypt_decrypt_stability(char *args);
extern int test_aes_main(char *args);
#endif
