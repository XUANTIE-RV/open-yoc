#include <string.h>
#include "se_device.h"
#include "se_aes.h"
#include "yunit.h"
#include "se_keystore.h"
#include "se_rng.h"
#include "sec_device.h"
#include "test.h"
#include "sec_crypto_aes.h"
#include "sec_crypto_rng.h"

static int sc_test_ecb_en_de_demo(sc_aes_key_bits_t key_len)
{
    sc_aes_t aes;

    uint32_t key[8] = {0x00};
    uint8_t in[128] ={0x00};
    uint8_t out_en[sizeof(in)]= {0x00};
    uint8_t out_de[sizeof(in)]= {0x00};
    uint32_t in_size = sizeof(in);
    uint32_t out_size = sizeof(in);
    uint32_t ret = 0;

    ret = sc_rng_get_multi_word(key, sizeof(key));
    CHECK_RET_WITH_RET(ret == 0, ret);

    ret = sc_aes_init(&aes, 0);
    CHECK_RET_WITH_RET(ret == 0, ret);

    ret = sc_aes_set_encrypt_key(&aes, key, key_len);
    CHECK_RET_WITH_RET(ret == 0, ret);

    ret = sc_aes_ecb_encrypt(&aes, in, out_en, in_size);
    CHECK_RET_WITH_RET(ret == 0, ret);

    ret = sc_aes_set_decrypt_key(&aes, key, key_len);
    CHECK_RET_WITH_RET(ret == 0, ret);

    ret = sc_aes_ecb_decrypt(&aes, out_en, out_de, out_size);
    CHECK_RET_WITH_RET(ret == 0, ret);

    ret = memcmp(in, out_de, out_size);
    CHECK_RET_WITH_RET(ret == 0, ret);

    sc_aes_uninit(&aes);
    
    return 0;
}

static int sc_test_cbc_en_de_demo(sc_aes_key_bits_t key_len)
{
    sc_aes_t aes;

    uint32_t key[8] = {0x00};
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

    ret = sc_rng_get_multi_word(key, sizeof(key));
    CHECK_RET_WITH_RET(ret == 0, ret);

    ret = sc_aes_init(&aes, 0);
    CHECK_RET_WITH_RET(ret == 0, ret);

    ret = sc_aes_set_encrypt_key(&aes, key, key_len);
    CHECK_RET_WITH_RET(ret == 0, ret);

    ret = sc_aes_cbc_encrypt(&aes, in, out_en, in_size, iv_tmp);
    CHECK_RET_WITH_RET(ret == 0, ret);

    ret = sc_aes_set_decrypt_key(&aes, key, key_len);
    CHECK_RET_WITH_RET(ret == 0, ret);

    ret = sc_aes_cbc_decrypt(&aes, out_en, out_de, out_size, iv_tmp);
    CHECK_RET_WITH_RET(ret == 0, ret);

    ret = memcmp(in, out_de, out_size);
    CHECK_RET_WITH_RET(ret == 0, ret);

    sc_aes_uninit(&aes);

    return 0;
}


static void sc_test_aes_demo(void)
{
    int ret = 0;

    printf("test aes ecb 128 encrypt and decrypt by command key\n");
    ret =  sc_test_ecb_en_de_demo(SE_AES_KEY_LEN_BITS_128);
    YUNIT_ASSERT_MSG_QA(ret == 0, "the sc_test_ecb_en_de_demo by command key is %d", ret);

    printf("test aes cbc 128 encrypt and decrypt by command key\n");
    ret =  sc_test_cbc_en_de_demo(SE_AES_KEY_LEN_BITS_128);
    YUNIT_ASSERT_MSG_QA(ret == 0, "the sc_test_cbc_en_de_demo by command key is %d", ret);

    printf("test aes ecb 256 encrypt and decrypt by command key\n");
    ret =  sc_test_ecb_en_de_demo(SE_AES_KEY_LEN_BITS_256);
    YUNIT_ASSERT_MSG_QA(ret == 0, "the sc_test_ecb_en_de_demo by command key is %d", ret);

    printf("test aes cbc 256 encrypt and decrypt by command key\n");
    ret =  sc_test_cbc_en_de_demo(SE_AES_KEY_LEN_BITS_256);
    YUNIT_ASSERT_MSG_QA(ret == 0, "the sc_test_cbc_en_de_demo by command key is %d", ret);
}

void sc_test_aes(void)
{
    /***************AES TEST START****************/

    add_yunit_test("sc_aes", &sc_test_aes_demo);

    /***************AES TEST END****************/
}

