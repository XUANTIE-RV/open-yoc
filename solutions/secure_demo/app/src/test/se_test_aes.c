#include <string.h>
#include "se_device.h"
#include "se_aes.h"
#include "yunit.h"
#include "se_keystore.h"
#include "se_rng.h"
#include "sec_device.h"
#include "test.h"


static int se_test_ecb_en_de_demo(se_aes_key_bits_t key_len, uint32_t key_index)
{
    se_aes_t aes;

    uint8_t key[32] = {0x00};
    uint8_t in[128] ={0x00};
    uint8_t out_en[sizeof(in)]= {0x00};
    uint8_t out_de[sizeof(in)]= {0x00};
    uint32_t in_size = sizeof(in);
    uint32_t out_size = sizeof(in);
    uint32_t ret = 0;

    ret = se_rng_get_bytes(key, sizeof(key));
    CHECK_RET_WITH_RET(ret == 0, ret);

    ret = se_aes_init(&aes);
    CHECK_RET_WITH_RET(ret == 0, ret);

    ret = se_aes_set_encrypt_key(&aes, key, key_len);
    CHECK_RET_WITH_RET(ret == 0, ret);

    ret = se_aes_ecb_encrypt(&aes, in, out_en, in_size, key_index);
    CHECK_RET_WITH_RET(ret == 0, ret);

    ret = se_aes_set_decrypt_key(&aes, key, key_len);
    CHECK_RET_WITH_RET(ret == 0, ret);

    ret = se_aes_ecb_decrypt(&aes, out_en, out_de, out_size, key_index);
    CHECK_RET_WITH_RET(ret == 0, ret);

    ret = memcmp(in, out_de, out_size);
    CHECK_RET_WITH_RET(ret == 0, ret);

    se_aes_uninit(&aes);
    
    return 0;
}

static int se_test_cbc_en_de_demo(se_aes_key_bits_t key_len, uint32_t key_index)
{
    se_aes_t aes;

    uint8_t key[32] = {0x00};
    uint8_t in[128] ={0x00};
    uint8_t out_en[sizeof(in)]= {0x00};
    uint8_t out_de[sizeof(in)]= {0x00};
    uint32_t in_size = sizeof(in);
    uint32_t out_size = sizeof(in);
    uint32_t ret = 0;
    uint8_t iv_data[16] = {
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 
        0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x01,
    };
    uint8_t iv_tmp[16] = {0};

    memcpy(iv_tmp, iv_data, 16);

    ret = se_rng_get_bytes(key, sizeof(key));
    CHECK_RET_WITH_RET(ret == 0, ret);

    ret = se_aes_init(&aes);
    CHECK_RET_WITH_RET(ret == 0, ret);

    ret = se_aes_set_encrypt_key(&aes, key, key_len);
    CHECK_RET_WITH_RET(ret == 0, ret);

    ret = se_aes_cbc_encrypt(&aes, in, out_en, in_size, iv_tmp, key_index);
    CHECK_RET_WITH_RET(ret == 0, ret);

    ret = se_aes_set_decrypt_key(&aes, key, key_len);
    CHECK_RET_WITH_RET(ret == 0, ret);

    ret = se_aes_cbc_decrypt(&aes, out_en, out_de, out_size, iv_tmp, key_index);
    CHECK_RET_WITH_RET(ret == 0, ret);

    ret = memcmp(in, out_de, out_size);
    CHECK_RET_WITH_RET(ret == 0, ret);

    se_aes_uninit(&aes);

    return 0;
}


static void se_test_aes_demo(void)
{
    int ret = 0;

    printf("test aes ecb 128 encrypt and decrypt by command key\n");
    ret =  se_test_ecb_en_de_demo(SE_AES_KEY_LEN_BITS_128, 0xFFFFFFFF);
    YUNIT_ASSERT_MSG_QA(ret == 0, "the se_test_ecb_en_de_demo by command key is %d", ret);

    printf("test aes ecb 128 encrypt and decrypt by internal key\n");
    ret =  se_test_ecb_en_de_demo(SE_AES_KEY_LEN_BITS_128, SE_AES_KEY_INDEX_1);
    YUNIT_ASSERT_MSG_QA(ret == 0, "the se_test_ecb_en_de_demo by internal key index 0x01 is %d", ret);

    printf("test aes ecb 256 encrypt and decrypt by command key\n");
    ret =  se_test_ecb_en_de_demo(SE_AES_KEY_LEN_BITS_256, 0xFFFFFFFF);
    YUNIT_ASSERT_MSG_QA(ret == 0, "the se_test_ecb_en_de_demo by command key is %d", ret);

    printf("test aes ecb 256 encrypt and decrypt by internal key\n");
    ret =  se_test_ecb_en_de_demo(SE_AES_KEY_LEN_BITS_256, SE_AES_KEY_INDEX_3);
    YUNIT_ASSERT_MSG_QA(ret == 0, "the se_test_ecb_en_de_demo by internal key index 0x01 is %d", ret);

    printf("test aes cbc 128 encrypt and decrypt by command key\n");
    ret =  se_test_cbc_en_de_demo(SE_AES_KEY_LEN_BITS_128, 0xFFFFFFFF);
    YUNIT_ASSERT_MSG_QA(ret == 0, "the se_test_cbc_en_de_demo by command key is %d", ret);

    printf("test aes cbc 128 encrypt and decrypt by internal key\n");
    ret =  se_test_cbc_en_de_demo(SE_AES_KEY_LEN_BITS_128, SE_AES_KEY_INDEX_1);
    YUNIT_ASSERT_MSG_QA(ret == 0, "the se_test_cbc_en_de_demo by internal key index 0x01 is %d", ret);

    printf("test aes cbc 256 encrypt and decrypt by command key\n");
    ret =  se_test_cbc_en_de_demo(SE_AES_KEY_LEN_BITS_256, 0xFFFFFFFF);
    YUNIT_ASSERT_MSG_QA(ret == 0, "the se_test_cbc_en_de_demo by command key is %d", ret);

    printf("test aes bc 256 encrypt and decrypt by internal key\n");
    ret =  se_test_cbc_en_de_demo(SE_AES_KEY_LEN_BITS_256, SE_AES_KEY_INDEX_3);
    YUNIT_ASSERT_MSG_QA(ret == 0, "the se_test_cbc_en_de_demo by internal key index 0x01 is %d", ret);

}

void se_test_aes(void)
{
    /***************AES TEST START****************/

    add_yunit_test("aes", &se_test_aes_demo);

    /***************AES TEST END****************/
}

