/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include "app_main.h"
#include "sec_crypto_aes.h"
#include "ulog/ulog.h"
#include <string.h>

#define AES_IDX 0


uint8_t key_128[16] = {0xDD, 0x34, 0x80, 0x8c, 0xE5, 0xE9, 0x75, 0x2C,
                       0x46, 0x68, 0x68, 0x95, 0x2B, 0xCD, 0x72, 0x39};  //定义128位密钥
//uint8_t key_128[16] = {DD34808cE5E9752C466868952BCD7239};
uint8_t key_192[24] = {0xDD, 0x34, 0x80, 0x8c, 0xE5, 0xE9, 0x75, 0x2C,
                       0x46, 0x68, 0x68, 0x95, 0x2B, 0xCD, 0x72, 0x39,
                       0x46, 0x68, 0x5E, 0x95, 0x4B, 0xCD, 0x72, 0x88,};  //定义192位密钥
//uint8_t key_192[24] = {DD34808cE5E9752C466868952BCD723946685E954BCD7288}；
uint8_t key_256[32] = {0xDD, 0x34, 0x80, 0x8c, 0xE5, 0xE9, 0x75, 0x2C,
                       0x46, 0x68, 0x68, 0x95, 0x2B, 0xCD, 0x72, 0x39,
                       0x46, 0x68, 0x5E, 0x95, 0x4B, 0xCD, 0x72, 0x88,
                       0x34, 0x80, 0x8c, 0xE5, 0x68, 0x68, 0x8c, 0xE5};  //定义256位密钥
//uint8_t key_256[32] = {DD34808cE5E9752C466868952BCD723946685E954BCD728834808cE568688cE5}; 


uint8_t plaintext_128[16] = {0x76, 0xCF, 0xFE, 0x1A, 0x85, 0x22, 0xFD, 0xCD,
                             0xFD, 0x12, 0xC8, 0x12, 0x87, 0xA8, 0x61, 0x30};//定义128位明文

uint8_t plaintext_120[15] = {0x76, 0xCF, 0xFE, 0x1A, 0x85, 0x22, 0xFD, 0xCD,
                             0xFD, 0x12, 0xC8, 0x12, 0x87, 0xA8, 0x61};//定义120位明文
//AES测试只与K长度有关
/*
uint8_t plaintext_192[24] = {0x76, 0xCF, 0xFE, 0x1A, 0x85, 0x22, 0xFD, 0xCD,
                             0xFD, 0x12, 0xC8, 0x12, 0x87, 0xA8, 0x61, 0x30,
                             0x46, 0x68, 0x5E, 0x95, 0x4B, 0xCD, 0x72, 0x88};//定义192位明文
uint8_t plaintext_256[32] = {0x76, 0xCF, 0xFE, 0x1A, 0x85, 0x22, 0xFD, 0xCD,
                             0xFD, 0x12, 0xC8, 0x12, 0x87, 0xA8, 0x61, 0x30,
                             0x46, 0x68, 0x5E, 0x95, 0x4B, 0xCD, 0x72, 0x88,
                             0xA0, 0x52, 0x3F, 0x54, 0x6D, 0xB0, 0xC0, 0xA3};//定义256位明文
//uint8_t plaintext_128[16] = {76CFFE1A8522FDCDFD12C81287A86130};
//uint8_t plaintext_192[24] = {76CFFE1A8522FDCDFD12C81287A8613046685E954BCD7288};
//uint8_t plaintext_256[32] = {76CFFE1A8522FDCDFD12C81287A8613046685E954BCD7288A0523F546DB0C0A3};
*/



uint8_t iv_128[16]={0xA9, 0xD5, 0x05, 0x45, 0x1A, 0x87, 0x8F, 0x41, 
                    0x3F, 0xFC, 0xC4, 0x30, 0xE4, 0x0D, 0xA1, 0x55};
//A9D505451A878F413FFCC430E40DA155
/*
ecb模式
uint8_t ciphertest_128[16]={0x85, 0xD0, 0x9D, 0x67, 0xB6, 0x2C, 0xCD, 0x70, 
                            0x6E, 0x99, 0x4A, 0xAC, 0x8A, 0x08, 0xC2, 0xE3};
uint8_t ciphertest_192[16]={0x4F, 0x2E, 0xAB, 0xF8, 0xAA, 0x43, 0x21, 0x0C, 
                            0x84, 0x6C, 0x71, 0xD0, 0x06, 0x63, 0x96, 0xC2};
uint8_t ciphertest_256[16]={0x54, 0x97, 0xA2, 0xE4, 0xD7, 0x98, 0xBE, 0x66, 
                            0x2D, 0x16, 0xB7, 0x77, 0xED, 0xF0, 0x74, 0x06};
//85D09D67B62CCD706E994AAC8A08C2E3
//4F2EABF8AA43210C846C71D0066396C2
//5497A2E4D798BE662D16B777EDF07406
*/




/*cbc模式
uint8_t ciphertest_128[16]={0xB0, 0x29, 0x22, 0x06, 0x7B, 0xF5, 0x97, 0x37, 
                            0x22, 0x50, 0xD3, 0xBF, 0x61, 0x33, 0xB3, 0x58};
uint8_t ciphertest_192[16]={0x4C, 0xF2, 0x02, 0x95, 0xEB, 0xD1, 0xC4, 0xB1,
                            0x4F, 0xBD, 0x1D, 0x0F, 0x69, 0xD1, 0x13, 0x77};
uint8_t ciphertest_256[16]={0xA9, 0x92, 0xE8, 0x27, 0x7C, 0x65, 0xDE, 0x0C,
                            0x3E, 0x4F, 0xE9, 0x77, 0xD5, 0x04, 0x72, 0xAD};
//B02922067BF597372250D3BF6133B358
//4CF20295EBD1C4B14FBD1D0F69D11377
//A992E8277C65DE0C3E4FE977D50472AD
*/


/*
 *  测试aes ecb模式128位key的加密、解密功能
 */
static int aes_ecb_128(void)
{
    uint8_t ciphertext_128[16]={0x85, 0xD0, 0x9D, 0x67, 0xB6, 0x2C, 0xCD, 0x70, 
                                0x6E, 0x99, 0x4A, 0xAC, 0x8A, 0x08, 0xC2, 0xE3};

    uint32_t ret;
    sc_aes_t aes_hdl;

    void *buffer = NULL;

    buffer = malloc(16);
    if (buffer == NULL) {
        printf("malloc error！\n");
        return -1;
    }

    ret = sc_aes_init(&aes_hdl, AES_IDX);
    CHECK_RET_WITHOUT_RET_P(ret == SC_OK, ret);

    ret = sc_aes_set_encrypt_key(&aes_hdl, key_128, SC_AES_KEY_LEN_BITS_128);
    CHECK_RET_WITHOUT_RET_P(ret == SC_OK, ret);

    ret = sc_aes_ecb_encrypt(&aes_hdl, plaintext_128, buffer, 16);
    CHECK_RET_WITHOUT_RET_P(ret == SC_OK, ret);

    ret = memcmp(ciphertext_128, buffer, 16);
    CHECK_RET_WITHOUT_RET_P(ret == 0, ret);


    ret = sc_aes_set_decrypt_key(&aes_hdl, key_128, SC_AES_KEY_LEN_BITS_128);
    CHECK_RET_WITHOUT_RET_P(ret == SC_OK, ret);

    ret = sc_aes_ecb_decrypt(&aes_hdl, ciphertext_128, buffer, 16);
    CHECK_RET_WITHOUT_RET_P(ret == SC_OK, ret);

    ret = memcmp(plaintext_128, buffer, 16);
    CHECK_RET_WITHOUT_RET_P(ret == 0, ret);

    printf("step1.\n");

    free(buffer);
    sc_aes_uninit(&aes_hdl);

    return 0;
}

/*
 *  测试aes ecb模式192位key的加密、解密功能
 */
static int aes_ecb_192(void)
{
    uint8_t ciphertext_192[16]={0x4F, 0x2E, 0xAB, 0xF8, 0xAA, 0x43, 0x21, 0x0C, 
                                0x84, 0x6C, 0x71, 0xD0, 0x06, 0x63, 0x96, 0xC2};


    uint32_t ret;
    sc_aes_t aes_hdl;

    void *buffer = NULL;

    buffer = malloc(16);
    if (buffer == NULL) {
        printf("malloc error！\n");
        return -1;
    }

    ret = sc_aes_init(&aes_hdl, AES_IDX);
    CHECK_RET_WITHOUT_RET_P(ret == SC_OK, ret);

    ret = sc_aes_set_encrypt_key(&aes_hdl, key_192, SC_AES_KEY_LEN_BITS_192);
    CHECK_RET_WITHOUT_RET_P(ret == SC_OK, ret);

    ret = sc_aes_ecb_encrypt(&aes_hdl, plaintext_128, buffer, 16);
    CHECK_RET_WITHOUT_RET_P(ret == SC_OK, ret);

    ret = memcmp(ciphertext_192, buffer, 16);
    CHECK_RET_WITHOUT_RET_P(ret == 0, ret);


    ret = sc_aes_set_decrypt_key(&aes_hdl, key_192, SC_AES_KEY_LEN_BITS_192);
    CHECK_RET_WITHOUT_RET_P(ret == SC_OK, ret);

    ret = sc_aes_ecb_decrypt(&aes_hdl, ciphertext_192, buffer, 16);
    CHECK_RET_WITHOUT_RET_P(ret == SC_OK, ret);

    ret = memcmp(plaintext_128, buffer, 16);
    CHECK_RET_WITHOUT_RET_P(ret == 0, ret);

    printf("step2.\n");

    free(buffer);
    sc_aes_uninit(&aes_hdl);
    return 0;
}

/*
 *  测试aes ecb模式256位key的加密、解密功能
 */
static int aes_ecb_256(void)
{
    uint8_t ciphertext_256[16]={0x54, 0x97, 0xA2, 0xE4, 0xD7, 0x98, 0xBE, 0x66, 
                                0x2D, 0x16, 0xB7, 0x77, 0xED, 0xF0, 0x74, 0x06};
    uint32_t ret;
    sc_aes_t aes_hdl;

    void *buffer = NULL;

    buffer = malloc(16);
    if (buffer == NULL) {
        printf("malloc error！\n");
        return -1;
    }

    ret = sc_aes_init(&aes_hdl, AES_IDX);
    CHECK_RET_WITHOUT_RET_P(ret == SC_OK, ret);

    ret = sc_aes_set_encrypt_key(&aes_hdl, key_256, SC_AES_KEY_LEN_BITS_256);
    CHECK_RET_WITHOUT_RET_P(ret == SC_OK, ret);

    ret = sc_aes_ecb_encrypt(&aes_hdl, plaintext_128, buffer, 16);
    CHECK_RET_WITHOUT_RET_P(ret == SC_OK, ret);

    ret = memcmp(ciphertext_256, buffer, 16);
    CHECK_RET_WITHOUT_RET_P(ret == 0, ret);


    ret = sc_aes_set_decrypt_key(&aes_hdl, key_256, SC_AES_KEY_LEN_BITS_256);
    CHECK_RET_WITHOUT_RET_P(ret == SC_OK, ret);

    ret = sc_aes_ecb_decrypt(&aes_hdl, ciphertext_256, buffer, 16);
    CHECK_RET_WITHOUT_RET_P(ret == SC_OK, ret);

    ret = memcmp(plaintext_128, buffer, 16);
    CHECK_RET_WITHOUT_RET_P(ret == 0, ret);

    printf("step3.\n");
    printf("===%s, %d, %x\n", __FUNCTION__, __LINE__, 0);
    aos_log_hexdump("iv", (char *)iv_128, 16);

    free(buffer);
    sc_aes_uninit(&aes_hdl);

    return 0;
}


/*
 *  测试aes cbc模式128位key的加密、解密功能
 */
static int aes_cbc_128(void)
{
    uint8_t ciphertext_128[16]={0xB0, 0x29, 0x22, 0x06, 0x7B, 0xF5, 0x97, 0x37, 
                                0x22, 0x50, 0xD3, 0xBF, 0x61, 0x33, 0xB3, 0x58};

    uint32_t ret;
    sc_aes_t aes_hdl;

    void *buffer = NULL;

    buffer = malloc(16);
    if (buffer == NULL) {
        printf("malloc error！\n");
        return -1;
    }

    printf("===%s, %d, %x\n", __FUNCTION__, __LINE__, 0);
    aos_log_hexdump("iv", (char *)iv_128, 16);
    ret = sc_aes_init(&aes_hdl, AES_IDX);
    CHECK_RET_WITHOUT_RET_P(ret == SC_OK, ret);

    printf("===%s, %d, %x\n", __FUNCTION__, __LINE__, 0);
    aos_log_hexdump("iv", (char *)iv_128, 16);
    ret = sc_aes_set_encrypt_key(&aes_hdl, key_128, SC_AES_KEY_LEN_BITS_128);
    CHECK_RET_WITHOUT_RET_P(ret == SC_OK, ret);

    printf("===%s, %d, %x\n", __FUNCTION__, __LINE__, 0);
    aos_log_hexdump("iv", (char *)iv_128, 16);
    ret = sc_aes_cbc_encrypt(&aes_hdl, plaintext_128, buffer, 16,iv_128);
    CHECK_RET_WITHOUT_RET_P(ret == SC_OK, ret);

    printf("===%s, %d, %x\n", __FUNCTION__, __LINE__, 0);
    aos_log_hexdump("iv", (char *)iv_128, 16);
    ret = memcmp(ciphertext_128, buffer, 16);
    CHECK_RET_WITHOUT_RET_P(ret == 0, ret);


    ret = sc_aes_set_decrypt_key(&aes_hdl, key_128, SC_AES_KEY_LEN_BITS_128);
    CHECK_RET_WITHOUT_RET_P(ret == SC_OK, ret);

    printf("===%s, %d, %x\n", __FUNCTION__, __LINE__, 0);
    aos_log_hexdump("iv", (char *)iv_128, 16);
    ret = sc_aes_cbc_decrypt(&aes_hdl, ciphertext_128, buffer, 16,iv_128);
    CHECK_RET_WITHOUT_RET_P(ret == SC_OK, ret);
    printf("===%s, %d, %x\n", __FUNCTION__, __LINE__, 0);
    aos_log_hexdump("iv", (char *)iv_128, 16);

    ret = memcmp(plaintext_128, buffer, 16);
    CHECK_RET_WITHOUT_RET_P(ret == 0, ret);

    printf("step4.\n");

    free(buffer);
    sc_aes_uninit(&aes_hdl);
    return 0;
}

/*
 *  测试aes cbc模式192位key的加密、解密功能
 */
static int aes_cbc_192(void)
{

    uint8_t ciphertext_192[16]={0x4C, 0xF2, 0x02, 0x95, 0xEB, 0xD1, 0xC4, 0xB1,
                                0x4F, 0xBD, 0x1D, 0x0F, 0x69, 0xD1, 0x13, 0x77};

    uint32_t ret;
    sc_aes_t aes_hdl;

    void *buffer = NULL;

    buffer = malloc(16);
    if (buffer == NULL) {
        printf("malloc error！\n");
        return -1;
    }

    ret = sc_aes_init(&aes_hdl, AES_IDX);
    CHECK_RET_WITHOUT_RET_P(ret == SC_OK, ret);

    ret = sc_aes_set_encrypt_key(&aes_hdl, key_192, SC_AES_KEY_LEN_BITS_192);
    CHECK_RET_WITHOUT_RET_P(ret == SC_OK, ret);

    printf("===%s, %d, %x\n", __FUNCTION__, __LINE__, 0);
    aos_log_hexdump("iv", (char *)iv_128, 16);
    ret = sc_aes_cbc_encrypt(&aes_hdl, plaintext_128, buffer,16,iv_128);
    printf("===%s, %d, %x\n", __FUNCTION__, __LINE__, 0);
    aos_log_hexdump("iv", (char *)iv_128, 16);
    CHECK_RET_WITHOUT_RET_P(ret == SC_OK, ret);

    ret = memcmp(ciphertext_192, buffer, 16);
    CHECK_RET_WITHOUT_RET_P(ret == 0, ret);

    ret = sc_aes_set_decrypt_key(&aes_hdl, key_192, SC_AES_KEY_LEN_BITS_192);
    CHECK_RET_WITHOUT_RET_P(ret == SC_OK, ret);

    printf("===%s, %d, %x\n", __FUNCTION__, __LINE__, 0);
    aos_log_hexdump("iv", (char *)iv_128, 16);
    ret = sc_aes_cbc_decrypt(&aes_hdl, ciphertext_192, buffer,16,iv_128);
    printf("===%s, %d, %x\n", __FUNCTION__, __LINE__, 0);
    aos_log_hexdump("iv", (char *)iv_128, 16);
    CHECK_RET_WITHOUT_RET_P(ret == SC_OK, ret);

    ret = memcmp(plaintext_128, buffer, 16);
    CHECK_RET_WITHOUT_RET_P(ret == 0, ret);

    printf("step5.\n");

    free(buffer);
    sc_aes_uninit(&aes_hdl);
    return 0;
}

/*
 *  测试aes cbc模式256位key的加密、解密功能
 */
static int aes_cbc_256(void)
{
    uint8_t ciphertext_256[16]={0xA9, 0x92, 0xE8, 0x27, 0x7C, 0x65, 0xDE, 0x0C,
                                0x3E, 0x4F, 0xE9, 0x77, 0xD5, 0x04, 0x72, 0xAD};

    uint32_t ret;
    sc_aes_t aes_hdl;

    void *buffer = NULL;

    buffer = malloc(16);
    if (buffer == NULL) {
        printf("malloc error！\n");
        return -1;
    }

    ret = sc_aes_init(&aes_hdl, AES_IDX);
    CHECK_RET_WITHOUT_RET_P(ret == SC_OK, ret);

    ret = sc_aes_set_encrypt_key(&aes_hdl, key_256, SC_AES_KEY_LEN_BITS_256);
    CHECK_RET_WITHOUT_RET_P(ret == SC_OK, ret);

    ret = sc_aes_cbc_encrypt(&aes_hdl, plaintext_128, buffer, 16,iv_128);
    CHECK_RET_WITHOUT_RET_P(ret == SC_OK, ret);

    ret = memcmp(ciphertext_256, buffer, 16);
    CHECK_RET_WITHOUT_RET_P(ret == 0, ret);


    ret = sc_aes_set_decrypt_key(&aes_hdl, key_256, SC_AES_KEY_LEN_BITS_256);
    CHECK_RET_WITHOUT_RET_P(ret == SC_OK, ret);

    ret = sc_aes_cbc_decrypt(&aes_hdl, ciphertext_256, buffer, 16,iv_128);
    CHECK_RET_WITHOUT_RET_P(ret == SC_OK, ret);

    ret = memcmp(plaintext_128, buffer, 16);
    CHECK_RET_WITHOUT_RET_P(ret == 0, ret);

    printf("step6.\n");

    free(buffer);
    sc_aes_uninit(&aes_hdl);

    return 0;
}


/*
 *  测试aes ecb模式加密、解密时，指定的size不是128的整数倍
 */
static int aes_ecb_size(void)
{
    uint32_t ret;
    ret = sizeof(plaintext_120)*8%128;
    CHECK_RET_WITHOUT_RET_P(ret != 0, ret);

    printf("step7.\n");

    return 0;
}

/*
 *  测试aes cbc模式加密、解密时，指定的size不是128的整数倍
 */
static int aes_cbc_size(void)
{
    uint32_t ret;
    ret = sizeof(plaintext_120)*8%128;
    CHECK_RET_WITHOUT_RET_P(ret != 0, ret);

    printf("step8.\n");

    return 0;
}



void aes_test_main(void)
{
    printf("\nbegin to test AES..........\n");
    aes_ecb_128();
    aes_ecb_192();
    aes_ecb_256();
    aes_cbc_128();
    aes_cbc_192();
    aes_cbc_256();
    aes_ecb_size();
    aes_cbc_size();
    printf("\ntest AES done..........\n");

}