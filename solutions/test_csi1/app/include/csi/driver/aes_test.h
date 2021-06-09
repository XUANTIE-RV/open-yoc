#ifndef _AES_TEST_H_
#define _AES_TEST_H_
#include <stdint.h>
#include <drv/aes.h>
#include <soc.h>
#include <string.h>
#include <stdlib.h>
#include <autotest.h>
#include <test_log.h>
#include <test_config.h>
#include <test_common.h>


typedef struct{
    int32_t idx;
    uint8_t crypto_mode;//encrypt decrypt
    uint8_t aes_mode;//which mode 
    uint8_t key_len_bits;
    uint8_t endian_mode;
}test_aes_args_t;


extern test_aes_args_t test_args;
extern int test_aes_func(char *args);
extern int test_aes_interface(char *args);
extern int test_aes_main(char *args);

#endif