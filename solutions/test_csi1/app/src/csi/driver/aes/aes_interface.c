#include <aes_test.h>
#include <stdlib.h>

static void aes_callback()
{
    TEST_CASE_TIPS("enter aes cb.\n");
}
static uint8_t iv[16]={0xA9, 0xD5, 0x05, 0x45, 0x1A, 0x87, 0x8F, 0x41, 
                       0x3F, 0xFC, 0xC4, 0x30, 0xE4, 0x0D, 0xA1, 0x55};

static uint8_t plaintext[16] = {0x76, 0xCF, 0xFE, 0x1A, 0x85, 0x22, 0xFD, 0xCD,
                             0xFD, 0x12, 0xC8, 0x12, 0x87, 0xA8, 0x61, 0x30};//定义128位明文

static uint8_t key[3][16]={{0xDD, 0x34, 0x80, 0x8c, 0xE5, 0xE9, 0x75, 0x2C,
                        0x46, 0x68, 0x68, 0x95, 0x2B, 0xCD, 0x72, 0x39},
                       {0xDD, 0x34, 0x80, 0x8c, 0xE5, 0xE9, 0x75, 0x2C,
                        0x46, 0x68, 0x68, 0x95, 0x2B, 0xCD, 0x72, 0x39,
                        0x46, 0x68, 0x5E, 0x95, 0x4B, 0xCD, 0x72, 0x88},
                       {0xDD, 0x34, 0x80, 0x8c, 0xE5, 0xE9, 0x75, 0x2C,
                        0x46, 0x68, 0x68, 0x95, 0x2B, 0xCD, 0x72, 0x39,
                        0x46, 0x68, 0x5E, 0x95, 0x4B, 0xCD, 0x72, 0x88,
                        0x34, 0x80, 0x8c, 0xE5, 0x68, 0x68, 0x8c, 0xE5}};
static uint8_t ciphertext_ecb[3]={{0x85, 0xD0, 0x9D, 0x67, 0xB6, 0x2C, 0xCD, 0x70, 
                                   0x6E, 0x99, 0x4A, 0xAC, 0x8A, 0x08, 0xC2, 0xE3},
                                  {0x4F, 0x2E, 0xAB, 0xF8, 0xAA, 0x43, 0x21, 0x0C, 
                                   0x84, 0x6C, 0x71, 0xD0, 0x06, 0x63, 0x96, 0xC2},
                                  {0x54, 0x97, 0xA2, 0xE4, 0xD7, 0x98, 0xBE, 0x66, 
                                   0x2D, 0x16, 0xB7, 0x77, 0xED, 0xF0, 0x74, 0x06}};
static uint8_t ciphertext_cbc[3]={{0x85, 0xD0, 0x9D, 0x67, 0xB6, 0x2C, 0xCD, 0x70, 
                                   0x6E, 0x99, 0x4A, 0xAC, 0x8A, 0x08, 0xC2, 0xE3},
                                  {0x4F, 0x2E, 0xAB, 0xF8, 0xAA, 0x43, 0x21, 0x0C, 
                                   0x84, 0x6C, 0x71, 0xD0, 0x06, 0x63, 0x96, 0xC2},
                                  {0x54, 0x97, 0xA2, 0xE4, 0xD7, 0x98, 0xBE, 0x66, 
                                   0x2D, 0x16, 0xB7, 0x77, 0xED, 0xF0, 0x74, 0x06}};

static uint32_t aes_mode_array[7] = {AES_MODE_ECB,AES_MODE_CBC,AES_MODE_CFB1,AES_MODE_CFB8,AES_MODE_CFB128,AES_MODE_OFB,AES_MODE_CTR};
static uint32_t aes_crypto_mode_array[2] = {AES_CRYPTO_MODE_ENCRYPT,AES_CRYPTO_MODE_DECRYPT};
static uint32_t aes_key_len_bits[3] = {AES_KEY_LEN_BITS_128,AES_KEY_LEN_BITS_192,AES_KEY_LEN_BITS_256};
static uint32_t aes_endian_mode[2] = {AES_ENDIAN_LITTLE,AES_ENDIAN_BIG};

#if 0
uint8_t key_128[16] = {0xDD, 0x34, 0x80, 0x8c, 0xE5, 0xE9, 0x75, 0x2C,
                       0x46, 0x68, 0x68, 0x95, 0x2B, 0xCD, 0x72, 0x39};  //定义128位密钥
//uint8_t key_128[16] = {DD34808cE5E9752C466868952BCD7239};
uint8_t key_192[24] = {0xDD, 0x34, 0x80, 0x8c, 0xE5, 0xE9, 0x75, 0x2C,
                       0x46, 0x68, 0x68, 0x95, 0x2B, 0xCD, 0x72, 0x39,
                       0x46, 0x68, 0x5E, 0x95, 0x4B, 0xCD, 0x72, 0x88};  //定义192位密钥
//uint8_t key_192[24] = {DD34808cE5E9752C466868952BCD723946685E954BCD7288}；
uint8_t key_256[32] = {0xDD, 0x34, 0x80, 0x8c, 0xE5, 0xE9, 0x75, 0x2C,
                       0x46, 0x68, 0x68, 0x95, 0x2B, 0xCD, 0x72, 0x39,
                       0x46, 0x68, 0x5E, 0x95, 0x4B, 0xCD, 0x72, 0x88,
                       0x34, 0x80, 0x8c, 0xE5, 0x68, 0x68, 0x8c, 0xE5};  //定义256位密钥
//uint8_t key_256[32] = {DD34808cE5E9752C466868952BCD723946685E954BCD728834808cE568688cE5}; 
#endif

#if 0
static uint8_t ciphertext_ecb_128[16]={0x85, 0xD0, 0x9D, 0x67, 0xB6, 0x2C, 0xCD, 0x70, 
                                       0x6E, 0x99, 0x4A, 0xAC, 0x8A, 0x08, 0xC2, 0xE3};

static uint8_t ciphertext_ecb_192[16]={0x4F, 0x2E, 0xAB, 0xF8, 0xAA, 0x43, 0x21, 0x0C, 
                                       0x84, 0x6C, 0x71, 0xD0, 0x06, 0x63, 0x96, 0xC2};

static uint8_t ciphertext_ecb_256[16]={0x54, 0x97, 0xA2, 0xE4, 0xD7, 0x98, 0xBE, 0x66, 
                                       0x2D, 0x16, 0xB7, 0x77, 0xED, 0xF0, 0x74, 0x06};                               

static uint8_t ciphertext_cbc_128[16]={0x85, 0xD0, 0x9D, 0x67, 0xB6, 0x2C, 0xCD, 0x70, 
                                       0x6E, 0x99, 0x4A, 0xAC, 0x8A, 0x08, 0xC2, 0xE3};

static uint8_t ciphertext_cbc_192[16]={0x4F, 0x2E, 0xAB, 0xF8, 0xAA, 0x43, 0x21, 0x0C, 
                                       0x84, 0x6C, 0x71, 0xD0, 0x06, 0x63, 0x96, 0xC2};

static uint8_t ciphertext_cbc_256[16]={0x54, 0x97, 0xA2, 0xE4, 0xD7, 0x98, 0xBE, 0x66, 
                                       0x2D, 0x16, 0xB7, 0x77, 0xED, 0xF0, 0x74, 0x06};
#endif


#if 0
static uint8_t ciphertext_cfb1_128[]={};
static uint8_t ciphertext_cfb1_192[]={};
static uint8_t ciphertext_cfb1_256[]={};

static uint8_t ciphertext_cfb8_128[]={};
static uint8_t ciphertext_cfb8_192[]={};
static uint8_t ciphertext_cfb8_256[]={};

static uint8_t ciphertext_cfb128_128[]={};
static uint8_t ciphertext_cfb128_192[]={};
static uint8_t ciphertext_cfb128_256[]={};

static uint8_t ciphertext_ofb_128[]={};
static uint8_t ciphertext_ofb_192[]={};
static uint8_t ciphertext_ofb_256[]={};

static uint8_t ciphertext_ctr_128[]={};
static uint8_t ciphertext_ctr_192[]={};
static uint8_t ciphertext_ctr_256[]={};

#endif


int test_aes_interface(char *args)
{
    int32_t ret;
    aes_handle_t hd;
    test_aes_args_t td;
    aes_status_t aes_status;
    aes_capabilities_t aes_cap;

    uint8_t buffer[32];

    uint32_t num /*= sizeof()*/;

    hd = csi_aes_initialize(0,NULL);
    TEST_CASE_ASSERT_QUIT(hd != NULL,"aes initialize failed.\n");

    ret = csi_aes_power_control(NULL, DRV_POWER_FULL);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_AES_BASE | DRV_ERROR_UNSUPPORTED),"error");

    aes_cap = csi_aes_get_capabilities(0);
//    TEST_CASE_ASSERT(aes_cap ！= NULL,"aes get capabilities failed.\n");

    ret = csi_aes_uninitialize(NULL);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_AES_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_AES_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);

//int32_t csi_aes_config(aes_handle_t handle,aes_mode_e mode,aes_key_len_bits_e keylen_bits,aes_endian_mode_e endian);
    ret = csi_aes_config(NULL,aes_mode_array[1],aes_key_len_bits[1],aes_endian_mode[1]);
    TEST_CASE_ASSERT_QUIT(ret == (CSI_DRV_ERRNO_AES_BASE | DRV_ERROR_PARAMETER),"aes config failed.\n");

//int32_t csi_aes_set_key(aes_handle_t handle, void *context, void *key, aes_key_len_bits_e key_len, aes_crypto_mode_e enc);
    ret = csi_aes_set_key(NULL,NULL,key[1],aes_key_len_bits[1],aes_crypto_mode_array[0]);
    TEST_CASE_ASSERT_QUIT(ret == (CSI_DRV_ERRNO_AES_BASE | DRV_ERROR_PARAMETER),"aes set key failed.\n");

//int32_t csi_aes_ecb_crypto(aes_handle_t handle, void *context, void *in, void *out, uint32_t len);
    ret = csi_aes_ecb_crypto(NULL,NULL,plaintext,buffer,sizeof(plaintext));
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_AES_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_AES_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);

//int32_t csi_aes_cbc_crypto(aes_handle_t handle, void *context, void *in, void *out, uint32_t len, uint8_t iv[16]);

    ret = csi_aes_cbc_crypto(NULL,NULL,plaintext,buffer,sizeof(plaintext),iv[16]);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_AES_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_AES_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);

#if 0
//int32_t csi_aes_cfb1_crypto(aes_handle_t handle, void *context, void *in, void *out,  uint32_t len, uint8_t iv[16]);

    ret = csi_aes_cfb1_crypto(NULL,NULL,plaintext,buffer,sizeof(plaintext),iv[16]);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_AES_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_AES_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);

//int32_t csi_aes_cfb8_crypto(aes_handle_t handle, void *context, void *in, void *out, uint32_t len, uint8_t iv[16]);

    ret = csi_aes_cfb8_crypto(NULL,NULL,plaintext,buffer,sizeof(plaintext),iv[16]);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_AES_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_AES_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);

//int32_t csi_aes_cfb128_crypto(aes_handle_t handle, void *context, void *in, void *out, uint32_t len, uint8_t iv[16], uint32_t *num);

    ret = csi_aes_cfb128_crypto(NULL,NULL,plaintext,buffer,sizeof(plaintext),iv[16],&num);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_AES_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_AES_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);

//int32_t csi_aes_ofb_crypto(aes_handle_t handle, void *context, void *in, void *out, uint32_t len, uint8_t iv[16], uint32_t *num);

    ret = csi_aes_ofb_crypto(NULL,NULL,plaintext,buffer,sizeof(plaintext),iv[16],&num);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_AES_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_AES_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);

//int32_t csi_aes_ctr_crypto(aes_handle_t handle, void *context, void *in, void *out, uint32_t len, uint8_t nonce_counter[16], uint8_t stream_block[16], uint32_t *num);

//  ret = csi_aes_ctr_crypto(NULL,NULL,plaintext,buffer,sizeof(plaintext),iv[16]);//para error
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_AES_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_AES_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);
#endif


    aes_status = csi_aes_get_status(NULL);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_AES_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_WDT_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);

    ret =csi_aes_uninitialize(hd);
    return 0;

}



