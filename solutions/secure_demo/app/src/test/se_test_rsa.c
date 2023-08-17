#include <string.h>
#include "se_device.h"
#include "se_rsa.h"
#include "se_rng.h"
#include "yunit.h"
#include "se_keystore.h"

typedef struct _SE_RSA_TEST_S{
    se_rsa_padding_type_t padding_type;
    se_rsa_hash_type_t    hash_type;
    se_rsa_key_bits_t     key_bit_type;
    void *n;
    void *d;
    void *e;
    uint32_t size;
}SE_RSA_TEST_S;

static int se_test_rsa_encrypt_decrypt_demo(se_rsa_context_t *context, uint32_t key_index)
{
    int ret;
    se_rsa_t rsa;
    uint32_t en_len = 0;
    uint32_t de_len = 0;
    uint8_t  src_data[64] = {0};
    uint8_t  en_data[256] = {0};
    uint8_t  de_data[256] = {0};
    uint32_t src_size = sizeof(src_data);

    ret = se_rng_get_bytes(src_data, sizeof(src_data));
    CHECK_RET_WITH_RET(ret == 0, -1);

    ret = se_rsa_init(&rsa);
    CHECK_RET_WITH_RET(ret == 0, -1);

    ret = se_rsa_encrypt(&rsa, context, src_data, src_size, en_data, &en_len, key_index);
    CHECK_RET_WITH_RET(ret == 0, -1);

    ret = se_rsa_decrypt(&rsa, context, en_data, en_len, de_data, &de_len, key_index);
    CHECK_RET_WITH_RET(ret == 0, -1);

    se_rsa_uninit(&rsa);

    if ( (memcmp(src_data, de_data, de_len) != 0) || (de_len != src_size) ) {
        ret = -1;
        printf("de_len:%d src_size:%d \n", de_len, src_size);
        CHECK_RET_WITH_RET(ret == 0, -1);
    } 

    return 0;
}

static int se_test_rsa_sign_verify_demo(se_rsa_context_t *context, se_rsa_hash_type_t type, uint32_t key_index)
{
    int ret = 0;
    se_rsa_t rsa;
    uint8_t  sig_out[512] = {0};
    uint8_t  src_data[32] = {0};
    uint32_t src_size = sizeof(src_data);

    ret = se_rng_get_bytes(src_data, src_size);
    CHECK_RET_WITH_RET(ret == 0, -1);

    ret = se_rsa_init(&rsa);
    CHECK_RET_WITH_RET(ret == 0, -1);

    ret = se_rsa_sign(&rsa, context, (void *)src_data, src_size, sig_out, type, key_index);
    CHECK_RET_WITH_RET(ret == 0, -1);
    
    ret = se_rsa_verify(&rsa, context, (void*)src_data, src_size, (void *)sig_out, type, key_index);
    CHECK_RET_WITH_RET(ret == true, -1);

    se_rsa_uninit(&rsa);

    return 0;
}

static int se_test_rsa1024_encrypt_decrypt_demo(void)
{
    int ret;
    se_rsa_context_t context;
    
    for (int i = 0; i < 100; i++) {
        context.key_bits = SE_RSA_KEY_BITS_1024;
        context.padding_type = SE_RSA_PADDING_MODE_PKCS1;
        ret = se_test_rsa_encrypt_decrypt_demo(&context, SE_RSA_1024_KEY_INDEX_2);
        if (ret != 0) {
            printf("pressure failed!, cycle:%d\n", i);
            break;
        }
    }
    CHECK_RET_WITH_RET(ret == 0, -1);
    return 0;
}

static int se_test_rsa2048_encrypt_decrypt_demo(void)
{
    int ret;
    se_rsa_context_t context;
    
    for (int i = 0; i < 100; i++) {
        context.key_bits = SE_RSA_KEY_BITS_2048;
        context.padding_type = SE_RSA_PADDING_MODE_PKCS1;
        ret = se_test_rsa_encrypt_decrypt_demo(&context, SE_RSA_2048_KEY_INDEX_2);
        if (ret != 0) {
            printf("pressure failed!, cycle:%d\n", i);
            break;
        }
    }
    CHECK_RET_WITH_RET(ret == 0, -1);
    return 0;
}


static int se_test_rsa1024_sign_verify_demo(void)
{
    int ret;
    se_rsa_context_t context;

    for (int i = 0; i < 100; i++) {
        context.key_bits = SE_RSA_KEY_BITS_1024;
        context.padding_type = SE_RSA_PADDING_MODE_PKCS1;
        ret = se_test_rsa_sign_verify_demo(&context, SE_RSA_HASH_TYPE_SHA256, SE_RSA_1024_KEY_INDEX_2);
        if (ret != 0) {
            printf("pressure failed!, cycle:%d\n", i);
            break;
        }
    }
    CHECK_RET_WITH_RET(ret == 0, -1);
    return 0;
}

static int se_test_rsa2048_sign_verify_demo(void)
{
    int ret;
    se_rsa_context_t context;

    for (int i = 0; i < 100; i++) {
        context.key_bits = SE_RSA_KEY_BITS_2048;
        context.padding_type = SE_RSA_PADDING_MODE_PKCS1;
        ret = se_test_rsa_sign_verify_demo(&context, SE_RSA_HASH_TYPE_SHA256, SE_RSA_2048_KEY_INDEX_2);
        if (ret != 0) {
            printf("pressure failed!, cycle:%d\n", i);
            break;
        }
    }
    CHECK_RET_WITH_RET(ret == 0, -1);
    return 0;
}


static void se_test_rsa_demo(void)
{
    int ret = 0;

    ret = se_test_rsa1024_encrypt_decrypt_demo();
    YUNIT_ASSERT_MSG_QA(ret == 0, "the se_test_rsa1024_encrypt_decrypt_demo is %d", ret);

    ret = se_test_rsa2048_encrypt_decrypt_demo();
    YUNIT_ASSERT_MSG_QA(ret == 0, "the se_test_rsa2048_encrypt_decrypt_demo is %d", ret);

    ret = se_test_rsa1024_sign_verify_demo();
    YUNIT_ASSERT_MSG_QA(ret == 0, "the se_test_rsa1024_sign_verify_demo is %d", ret);

    ret = se_test_rsa2048_sign_verify_demo();
    YUNIT_ASSERT_MSG_QA(ret == 0, "the se_test_rsa2048_sign_verify_demo is %d", ret);
}


void se_test_rsa(void)
{
    /***************RSA TEST START****************/

    add_yunit_test("rsa", &se_test_rsa_demo);

    /***************RSA TEST END****************/
}

