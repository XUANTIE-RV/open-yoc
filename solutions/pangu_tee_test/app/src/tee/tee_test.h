#ifndef __TEE_TEST_H__
#define __TEE_TEST_H__

#define CONFIG_ALGO_AES 1
#define CONFIG_ALGO_SHA 1
#define CONFIG_ALGO_RSA 1


void tee_aes_test(void);

void tee_cid_test(void);

void tee_mbedtls_test(void);

void tee_random_test(void);

void tee_rsa_test(void);

void tee_sha_test(void);

void tee_test(void);

#endif