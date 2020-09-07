#include "tee_test.h"
#include "tee_debug.h"

void tee_test(void)
{
    TEE_LOGI("tee test start----------------------\n");
    tee_cid_test();
    tee_random_test();
    tee_sha_test();
    tee_mbedtls_test();
    tee_aes_test();
    tee_rsa_test();
    TEE_LOGI("tee test end----------------------\n");
}