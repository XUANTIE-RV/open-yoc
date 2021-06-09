#include <aes_test.h>
#include <stdlib.h>

volatile static uint8_t aes_int_flag = 1;
static void aes_interrupt_handler(int32_t idx, aes_event_e event)
{
    aes_int_flag = 0;
}

static uint8_t iv[16]={0xA9, 0xD5, 0x05, 0x45, 0x1A, 0x87, 0x8F, 0x41, 
                       0x3F, 0xFC, 0xC4, 0x30, 0xE4, 0x0D, 0xA1, 0x55};
//A9D505451A878F413FFCC430E40DA155
static uint8_t plaintext[16] = {0x76, 0xCF, 0xFE, 0x1A, 0x85, 0x22, 0xFD, 0xCD,
                                0xFD, 0x12, 0xC8, 0x12, 0x87, 0xA8, 0x61, 0x30};//定义128位明文
//76CFFE1A8522FDCDFD12C81287A86130
//const uint8_t key_128[16] = "Demo-Key";
static uint8_t key_128[16]={0xDD, 0x34, 0x80, 0x8c, 0xE5, 0xE9, 0x75, 0x2C,
                            0x46, 0x68, 0x68, 0x95, 0x2B, 0xCD, 0x72, 0x39};
static uint8_t key_192[24]={0xDD, 0x34, 0x80, 0x8c, 0xE5, 0xE9, 0x75, 0x2C,
                            0x46, 0x68, 0x68, 0x95, 0x2B, 0xCD, 0x72, 0x39,
                            0x46, 0x68, 0x5E, 0x95, 0x4B, 0xCD, 0x72, 0x88};
static uint8_t key_256[32]={0xDD, 0x34, 0x80, 0x8c, 0xE5, 0xE9, 0x75, 0x2C,
                            0x46, 0x68, 0x68, 0x95, 0x2B, 0xCD, 0x72, 0x39,
                            0x46, 0x68, 0x5E, 0x95, 0x4B, 0xCD, 0x72, 0x88,
                            0x34, 0x80, 0x8c, 0xE5, 0x68, 0x68, 0x8c, 0xE5};

/*
//DD34808cE5E9752C466868952BCD7239
//DD34808cE5E9752C466868952BCD723946685E954BCD7288
//DD34808cE5E9752C466868952BCD723946685E954BCD728834808cE568688cE5
*/
static uint8_t ciphertext_ecb_128[16]={0x85, 0xD0, 0x9D, 0x67, 0xB6, 0x2C, 0xCD, 0x70, 
                                       0x6E, 0x99, 0x4A, 0xAC, 0x8A, 0x08, 0xC2, 0xE3};
static uint8_t ciphertext_ecb_192[16]={0x4F, 0x2E, 0xAB, 0xF8, 0xAA, 0x43, 0x21, 0x0C, 
                                       0x84, 0x6C, 0x71, 0xD0, 0x06, 0x63, 0x96, 0xC2};
static uint8_t ciphertext_ecb_256[16]={0x54, 0x97, 0xA2, 0xE4, 0xD7, 0x98, 0xBE, 0x66, 
                                       0x2D, 0x16, 0xB7, 0x77, 0xED, 0xF0, 0x74, 0x06};

static uint8_t ciphertext_ecb_128_little[16]={0x67, 0x9D, 0xD0, 0x85, 0x70, 0xCD, 0x2C, 0xB6, 0xAC, 0x4A, 0x99, 0x6E, 0xE3, 0xC2, 0x08, 0x8A};
static uint8_t ciphertext_ecb_192_little[16]={0xF8, 0xAB, 0x2E, 0x4F,0x0C, 0x21, 0x43, 0xAA,0xD0, 0x71, 0x6C, 0x84,0xC2, 0x96, 0x63, 0x06};
static uint8_t ciphertext_ecb_256_little[16]={0xE4, 0xA2, 0x97, 0x54, 0x66, 0xBE, 0x98, 0xD7, 0x77, 0xB7, 0x16, 0x2D, 0x06, 0x74, 0xF0, 0xED};
/*
//85D09D67B62CCD706E994AAC8A08C2E3
//4F2EABF8AA43210C846C71D0066396C2
//5497A2E4D798BE662D16B777EDF07406
*/
static uint8_t ciphertext_cbc_128[16]={0xB0, 0x29, 0x22, 0x06, 0x7B, 0xF5, 0x97, 0x37, 
                                       0x22, 0x50, 0xD3, 0xBF, 0x61, 0x33, 0xB3, 0x58};
static uint8_t ciphertext_cbc_192[16]={0x4C, 0xF2, 0x02, 0x95, 0xEB, 0xD1, 0xC4, 0xB1,
                                       0x4F, 0xBD, 0x1D, 0x0F, 0x69, 0xD1, 0x13, 0x77};
static uint8_t ciphertext_cbc_256[16]={0xA9, 0x92, 0xE8, 0x27, 0x7C, 0x65, 0xDE, 0x0C,
                                       0x3E, 0x4F, 0xE9, 0x77, 0xD5, 0x04, 0x72, 0xAD};

static uint8_t ciphertext_cbc_128_little[16]={0x06, 0x22, 0x29, 0xB0, 0x37, 0x97, 0xF5, 0x7B, 0xBF, 0xD3, 0x50, 0x22, 0x58, 0xB3, 0x33, 0x61};

static uint8_t ciphertext_cbc_192_little[16]={0x95, 0x02, 0xF2, 0x4C, 0xB1, 0xC4, 0xD1, 0xEB,0x0F, 0x1D, 0xBD, 0x4F,0x77, 0x13, 0xD1, 0x69};

static uint8_t ciphertext_cbc_256_little[16]={0x27, 0xE8, 0x92, 0xA9, 	0x0C, 0xDE, 0x65, 0x7C,0x77, 0xE9, 0x4F, 0x3E,0xAD, 0x72, 0x04, 0xD5};
/*
//B02922067BF597372250D3BF6133B358
//4CF20295EBD1C4B14FBD1D0F69D11377
//A992E8277C65DE0C3E4FE977D50472AD
*/
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


int test_aes_func(char *args)
{
    int32_t ret;
    aes_handle_t hd;
    test_aes_args_t td;
    aes_status_t aes_status;
    uint32_t str_length=sizeof(plaintext);

	td.idx = (uint8_t)*((uint32_t *)args);
    td.crypto_mode = (uint8_t)*((uint32_t*)args+1);//0 for encrypt,1 for decrypt
    td.aes_mode = (uint8_t)*((uint32_t *)args+2);
	td.key_len_bits = (uint8_t)*((uint32_t *)args+3);//key length:128 192 256
    td.endian_mode = (uint8_t)*((uint32_t *)args+4);//0 for little endian,1 for big endian

    TEST_CASE_TIPS("idx is %d\n",td.idx);
    TEST_CASE_TIPS("crypto_mode is %d\n",aes_crypto_mode_array[td.crypto_mode]);//encrypt decrypt
    TEST_CASE_TIPS("aes_mode is %d\n",aes_mode_array[td.aes_mode]);
    TEST_CASE_TIPS("key_len_bits is %d\n",aes_key_len_bits[td.key_len_bits]);
    TEST_CASE_TIPS("endian_mode is %d\n",aes_endian_mode[td.endian_mode]);   

    if(aes_crypto_mode_array[td.crypto_mode] == AES_CRYPTO_MODE_ENCRYPT){
        test_aes_mode_encrypt(td);
    }
    else if(aes_crypto_mode_array[td.crypto_mode] == AES_CRYPTO_MODE_DECRYPT){
        test_aes_mode_decrypt(td);
    }
    else{
        TEST_CASE_TIPS("PARA error\n");
    }
    return 0;
}


void test_aes_mode_encrypt(test_aes_args_t td)
{
    int32_t ret,i;
    aes_handle_t *hd=NULL;
    aes_status_t aes_status;
    void *buffer = NULL;



    buffer = malloc(16);
    if (buffer == NULL) {
        printf("malloc error！\n");
        return -1;
    }

    hd = csi_aes_initialize(td.idx,aes_interrupt_handler);
    TEST_CASE_ASSERT_QUIT(hd != NULL,"aes initialize failed.\n");

/*demo*/

    if(aes_key_len_bits[td.key_len_bits]==AES_KEY_LEN_BITS_128){

        ret = csi_aes_config(hd,aes_mode_array[td.aes_mode],AES_KEY_LEN_BITS_128,aes_endian_mode[td.endian_mode]);
        TEST_CASE_ASSERT_QUIT(ret == 0,"aes 128 config failed.\n");

        ret = csi_aes_set_key(hd,NULL,key_128,AES_KEY_LEN_BITS_128,AES_CRYPTO_MODE_ENCRYPT);
        TEST_CASE_ASSERT_QUIT(ret == 0,"aes set key failed.\n");
        TEST_CASE_TIPS("csi_aes_set_key:128"); 

    }
    else if(aes_key_len_bits[td.key_len_bits]==AES_KEY_LEN_BITS_192){

        ret = csi_aes_config(hd,aes_mode_array[td.aes_mode],AES_KEY_LEN_BITS_192,aes_endian_mode[td.endian_mode]);
        TEST_CASE_ASSERT_QUIT(ret == 0,"aes 192 config failed.\n");

        ret = csi_aes_set_key(hd,NULL,key_192,AES_KEY_LEN_BITS_192,AES_CRYPTO_MODE_ENCRYPT);
        TEST_CASE_ASSERT_QUIT(ret == 0,"aes set key failed.\n");
        TEST_CASE_TIPS("csi_aes_set_key:192"); 

    }
    else if(aes_key_len_bits[td.key_len_bits]==AES_KEY_LEN_BITS_256){
       
        TEST_CASE_TIPS("little endian done\n");
        ret = csi_aes_config(hd,aes_mode_array[td.aes_mode],AES_KEY_LEN_BITS_256,aes_endian_mode[td.endian_mode]);
        TEST_CASE_ASSERT_QUIT(ret == 0,"aes 256 config failed.\n");

        ret = csi_aes_set_key(hd,NULL,key_256,AES_KEY_LEN_BITS_256,AES_CRYPTO_MODE_ENCRYPT);
        TEST_CASE_ASSERT_QUIT(ret == 0,"aes set key failed.\n");
        TEST_CASE_TIPS("csi_aes_set_key:256"); 
    }
    else{
        TEST_CASE_TIPS("Para error.\n");
    }

    TEST_CASE_TIPS("x=%d\n",aes_mode_array[td.aes_mode]);
    TEST_CASE_TIPS("y=%d\n",aes_key_len_bits[td.key_len_bits]);
    TEST_CASE_TIPS("z=%d\n",aes_endian_mode[td.endian_mode]);
    TEST_CASE_TIPS("aes config and set_key ok");

    switch(aes_mode_array[td.aes_mode])
    {
        case AES_MODE_ECB:
//int32_t csi_aes_ecb_crypto(aes_handle_t handle, void *context, void *in, void *out, uint32_t len);
            ret = csi_aes_ecb_crypto(hd,NULL,plaintext,buffer,sizeof(plaintext));
            TEST_CASE_TIPS("ret = %d\n",ret);
            TEST_CASE_ASSERT_QUIT(ret == 0,"aes mode:ecb failed.actual_ret is %d\n\n",ret);
            break;
        case AES_MODE_CBC:
//int32_t csi_aes_cbc_crypto(aes_handle_t handle, void *context, void *in, void *out, uint32_t len, uint8_t iv[16]);

            ret = csi_aes_cbc_crypto(hd,NULL,plaintext,buffer,sizeof(plaintext),iv[16]);
            TEST_CASE_ASSERT_QUIT(ret == 0,"aes mode:cbc failed.\n");
            break;
#if 0
        case AES_MODE_CFB1:
//int32_t csi_aes_cfb1_crypto(aes_handle_t handle, void *context, void *in, void *out,  uint32_t len, uint8_t iv[16]);

            ret = csi_aes_cfb1_crypto(hd,NULL,plaintext,buffer,sizeof(plaintext),iv[16]);
            TEST_CASE_ASSERT_QUIT(ret == 0,"aes mode:cfb1 failed.\n");
            break;
        case AES_MODE_CFB8:
//int32_t csi_aes_cfb8_crypto(aes_handle_t handle, void *context, void *in, void *out, uint32_t len, uint8_t iv[16]);

            ret = csi_aes_cfb8_crypto(hd,NULL,plaintext,buffer,sizeof(plaintext),iv[16]);
            TEST_CASE_ASSERT_QUIT(ret == 0,"aes mode:cfb8 failed.\n");
            break;
        case AES_MODE_CFB128:
//int32_t csi_aes_cfb128_crypto(aes_handle_t handle, void *context, void *in, void *out, uint32_t len, uint8_t iv[16], uint32_t *num);

            ret = csi_aes_cfb128_crypto(hd,NULL,plaintext,buffer,sizeof(plaintext),iv[16],&num);
            TEST_CASE_ASSERT_QUIT(ret == 0,"aes mode:cfb128 failed.\n");
            break;
        case AES_MODE_OFB:
//int32_t csi_aes_ofb_crypto(aes_handle_t handle, void *context, void *in, void *out, uint32_t len, uint8_t iv[16], uint32_t *num);

            ret = csi_aes_ofb_crypto(hd,NULL,plaintext,buffer,sizeof(plaintext),iv[16],&num);
            TEST_CASE_ASSERT_QUIT(ret == 0,"aes mode:ofb failed.\n");
            break;
        case AES_MODE_CTR:
//int32_t csi_aes_ctr_crypto(aes_handle_t handle, void *context, void *in, void *out, uint32_t len, uint8_t nonce_counter[16], uint8_t stream_block[16], uint32_t *num);

//            ret = csi_aes_ctr_crypto(hd,NULL,plaintext,buffer,sizeof(plaintext),iv[16]);//para error
            TEST_CASE_ASSERT_QUIT(ret == 0,"aes mode:ctr failed.\n");
            break;
#endif
        default:
            TEST_CASE_TIPS("PARA ERROR\n");
            break;

    }
    while(aes_status.busy != 0){
        aes_status = csi_aes_get_status(hd);
    }

    TEST_CASE_TIPS("csi_aes_get_status ok");

    switch (aes_mode_array[td.aes_mode])
    {
    case AES_MODE_ECB:
        switch (aes_endian_mode[td.endian_mode])
        {
            case AES_ENDIAN_LITTLE:
                switch(aes_key_len_bits[td.key_len_bits]){
                    case AES_KEY_LEN_BITS_128:
                        ret = memcmp(ciphertext_ecb_128_little,buffer,16);
                        TEST_CASE_ASSERT_QUIT(ret == 0,"aes crypto:encrypt failed.\n");
                        break;
                    case AES_KEY_LEN_BITS_192:
                        ret = memcmp(ciphertext_ecb_192_little,buffer,16);
                        TEST_CASE_ASSERT_QUIT(ret == 0,"aes crypto:encrypt failed.\n");
                        break;
                    case AES_KEY_LEN_BITS_256:
                        ret = memcmp(ciphertext_ecb_256_little,buffer,16);
                        TEST_CASE_ASSERT_QUIT(ret == 0,"aes crypto:encrypt failed.\n");
                        break;
                    default:
                        TEST_CASE_TIPS("PARA ERROR\n");
                }
                break;
            case AES_ENDIAN_BIG:
                switch(aes_key_len_bits[td.key_len_bits]){
                    case AES_KEY_LEN_BITS_128:
                        ret = memcmp(ciphertext_ecb_128,buffer,16);
                        TEST_CASE_ASSERT_QUIT(ret == 0,"aes crypto:encrypt failed.\n");
                        break;
                    case AES_KEY_LEN_BITS_192:
                        ret = memcmp(ciphertext_ecb_192,buffer,16);
                        TEST_CASE_ASSERT_QUIT(ret == 0,"aes crypto:encrypt failed.\n");
                        break;
                    case AES_KEY_LEN_BITS_256:
                        ret = memcmp(ciphertext_ecb_256,buffer,16);
                        TEST_CASE_ASSERT_QUIT(ret == 0,"aes crypto:encrypt failed.\n");
                        break;
                    default:
                        TEST_CASE_TIPS("PARA ERROR\n");
                        break;
                }
                break;
            default:
                TEST_CASE_TIPS("PARA ERROR\n");       
                break;
        }
        break;
    case AES_MODE_CBC:
        switch (aes_endian_mode[td.endian_mode])
        {
            case AES_ENDIAN_LITTLE:
                switch(aes_key_len_bits[td.key_len_bits]){
                    case AES_KEY_LEN_BITS_128:
                        ret = memcmp(ciphertext_cbc_128_little,buffer,16);
                        TEST_CASE_ASSERT_QUIT(ret == 0,"aes crypto:encrypt failed.\n");
                        break;
                    case AES_KEY_LEN_BITS_192:
                        ret = memcmp(ciphertext_cbc_192_little,buffer,16);
                        TEST_CASE_ASSERT_QUIT(ret == 0,"aes crypto:encrypt failed.\n");
                        break;
                    case AES_KEY_LEN_BITS_256:
                        ret = memcmp(ciphertext_cbc_256_little,buffer,16);
                        TEST_CASE_ASSERT_QUIT(ret == 0,"aes crypto:encrypt failed.\n");
                        break;
                    default:
                        TEST_CASE_TIPS("PARA ERROR\n");
                }
                break;
            case AES_ENDIAN_BIG:
                switch(aes_key_len_bits[td.key_len_bits]){
                    case AES_KEY_LEN_BITS_128:
                        ret = memcmp(ciphertext_cbc_128,buffer,16);
                        TEST_CASE_ASSERT_QUIT(ret == 0,"aes crypto:encrypt failed.\n");
                        break;
                    case AES_KEY_LEN_BITS_192:
                        ret = memcmp(ciphertext_cbc_192,buffer,16);
                        TEST_CASE_ASSERT_QUIT(ret == 0,"aes crypto:encrypt failed.\n");
                        break;
                    case AES_KEY_LEN_BITS_256:
                        ret = memcmp(ciphertext_cbc_256,buffer,16);
                        TEST_CASE_ASSERT_QUIT(ret == 0,"aes crypto:encrypt failed.\n");
                        break;
                    default:
                        TEST_CASE_TIPS("PARA ERROR\n");
                        break;
                }
                break;    
            default:
                TEST_CASE_TIPS("PARA ERROR\n");
                break;
        }
        break;
    default:
        TEST_CASE_TIPS("PARA ERROR\n");
        break;
    }

    ret =csi_aes_uninitialize(hd);
    TEST_CASE_ASSERT_QUIT(ret == 0,"aes uninitialize failed.\n");

    free(buffer);
}



void test_aes_mode_decrypt(test_aes_args_t td)
{
    int32_t ret;
    aes_handle_t hd;
    aes_status_t aes_status;
    void *buffer = NULL;

    uint32_t num/* = sizeof() */;

    buffer = malloc(32);
    if (buffer == NULL) {
        printf("malloc error！\n");
        return -1;
    }

    hd = csi_aes_initialize(td.idx,NULL);
    TEST_CASE_ASSERT_QUIT(hd != NULL,"aes initialize failed.\n");
    TEST_CASE_TIPS("initialize ok");

    if(aes_key_len_bits[td.key_len_bits]==AES_KEY_LEN_BITS_128){
        ret = csi_aes_config(hd,aes_mode_array[td.aes_mode],AES_KEY_LEN_BITS_128,aes_endian_mode[td.endian_mode]);
        TEST_CASE_ASSERT_QUIT(ret == 0,"aes 128 config failed.\n");

        ret = csi_aes_set_key(hd,NULL,key_128,AES_KEY_LEN_BITS_128,AES_CRYPTO_MODE_DECRYPT);
        TEST_CASE_ASSERT_QUIT(ret == 0,"aes set key failed.\n");
        TEST_CASE_TIPS("csi_aes_set_key:128"); 

        switch (aes_mode_array[td.aes_mode])
        {
        case AES_MODE_ECB:
            ret = csi_aes_ecb_crypto(hd,NULL,ciphertext_ecb_128,buffer,sizeof(ciphertext_ecb_128));
            TEST_CASE_ASSERT_QUIT(ret == 0,"aes mode:cbc failed,actual_ret is %d\n",ret);

            ret = memcmp(ciphertext_ecb_128,buffer,sizeof(ciphertext_ecb_128));
            TEST_CASE_ASSERT_QUIT(ret == 0,"aes decrypt failed.\n");
            break;
        case AES_MODE_CBC:
            ret = csi_aes_cbc_crypto(hd,NULL,ciphertext_cbc_128,buffer,sizeof(ciphertext_cbc_128),iv[16]);
            TEST_CASE_ASSERT_QUIT(ret == 0,"aes mode:cbc failed,actual_ret is %d\n",ret);

            ret = memcmp(ciphertext_cbc_128,buffer,sizeof(ciphertext_cbc_128));
            TEST_CASE_ASSERT_QUIT(ret == 0,"aes decrypt failed.\n");
            break;
        case AES_MODE_CFB1:
        case AES_MODE_CFB8:
        case AES_MODE_CFB128:
        case AES_MODE_OFB:
        case AES_MODE_CTR:
        
        default:
        TEST_CASE_TIPS("para error");
            break;
        }


    }
    else if(aes_key_len_bits[td.key_len_bits]==AES_KEY_LEN_BITS_192){
        ret = csi_aes_config(hd,aes_mode_array[td.aes_mode],AES_KEY_LEN_BITS_192,aes_endian_mode[td.endian_mode]);
        TEST_CASE_ASSERT_QUIT(ret == 0,"aes 192 config failed.\n");

        ret = csi_aes_set_key(hd,NULL,key_192,AES_KEY_LEN_BITS_192,AES_CRYPTO_MODE_DECRYPT);
        TEST_CASE_ASSERT_QUIT(ret == 0,"aes set key failed.\n");
        TEST_CASE_TIPS("csi_aes_set_key:192");
        switch (aes_mode_array[td.aes_mode])
        {
            case AES_MODE_ECB:
                ret = csi_aes_ecb_crypto(hd,NULL,ciphertext_ecb_192,buffer,sizeof(ciphertext_ecb_192));
                TEST_CASE_ASSERT_QUIT(ret == 0,"aes mode:cbc failed,actual_ret is %d\n",ret);

                ret = memcmp(ciphertext_ecb_192,buffer,sizeof(ciphertext_ecb_192));
                TEST_CASE_ASSERT_QUIT(ret == 0,"aes decrypt failed.\n");
                break;
            case AES_MODE_CBC:
                ret = csi_aes_cbc_crypto(hd,NULL,ciphertext_cbc_192,buffer,sizeof(ciphertext_cbc_192),iv[16]);
                TEST_CASE_ASSERT_QUIT(ret == 0,"aes mode:cbc failed,actual_ret is %d\n",ret);

                ret = memcmp(ciphertext_cbc_192,buffer,sizeof(ciphertext_cbc_192));
                TEST_CASE_ASSERT_QUIT(ret == 0,"aes decrypt failed.\n");
                break;
#if 0
            case AES_MODE_CFB1:
                break;
            case AES_MODE_CFB8:
                break;
            case AES_MODE_CFB128:
                break;
            case AES_MODE_OFB:
                break;
            case AES_MODE_CTR:
                break;
#endif
            default:
                TEST_CASE_TIPS("para error");
                break;
        } 

    }
    else if(aes_key_len_bits[td.key_len_bits]==AES_KEY_LEN_BITS_256)
    {
        ret = csi_aes_config(hd,aes_mode_array[td.aes_mode],AES_KEY_LEN_BITS_256,aes_endian_mode[td.endian_mode]);
        TEST_CASE_ASSERT_QUIT(ret == 0,"aes 256 config failed.\n");

        ret = csi_aes_set_key(hd,NULL,key_256,AES_KEY_LEN_BITS_256,AES_CRYPTO_MODE_DECRYPT);
        TEST_CASE_ASSERT_QUIT(ret == 0,"aes set key failed.\n");
        TEST_CASE_TIPS("csi_aes_set_key:256"); 
        switch (aes_mode_array[td.aes_mode])
        {
            case AES_MODE_ECB:
                ret = csi_aes_ecb_crypto(hd,NULL,ciphertext_ecb_256,buffer,sizeof(ciphertext_ecb_256));
                TEST_CASE_ASSERT_QUIT(ret == 0,"aes mode:cbc failed,actual_ret is %d\n",ret);

                ret = memcmp(ciphertext_ecb_256,buffer,sizeof(ciphertext_ecb_256));
                TEST_CASE_ASSERT_QUIT(ret == 0,"aes decrypt failed.\n");
                break;
            case AES_MODE_CBC:
                ret = csi_aes_cbc_crypto(hd,NULL,ciphertext_cbc_256,buffer,sizeof(ciphertext_cbc_256),iv[16]);
                TEST_CASE_ASSERT_QUIT(ret == 0,"aes mode:cbc failed,actual_ret is %d\n",ret);

                ret = memcmp(ciphertext_cbc_256,buffer,sizeof(ciphertext_cbc_256));
                TEST_CASE_ASSERT_QUIT(ret == 0,"aes decrypt failed.\n");
                break;
#if 0
            case AES_MODE_CFB1:
                break;
            case AES_MODE_CFB8:
                break;
            case AES_MODE_CFB128:
                break;
            case AES_MODE_OFB:
                break;
            case AES_MODE_CTR:
                break;
#endif
            default:
            TEST_CASE_TIPS("para error");
                break;
        }
    }
    else{
        TEST_CASE_TIPS("Para error.\n");
    }

//    TEST_CASE_TIPS("x=%d\n",aes_mode_array[td.aes_mode]);
//    TEST_CASE_TIPS("y=%d\n",aes_key_len_bits[td.key_len_bits]);
//    TEST_CASE_TIPS("z=%d\n",aes_endian_mode[td.endian_mode]);
//    TEST_CASE_TIPS("aes config and set_key ok");

    TEST_CASE_TIPS("aes config and set_key ok");
    aes_status = csi_aes_get_status(hd);
    TEST_CASE_ASSERT_QUIT(aes_status.busy == 0,"aes get status error.\n");

    ret =csi_aes_uninitialize(hd);
    TEST_CASE_ASSERT_QUIT(ret == 0,"aes uninitialize failed.\n");

    free(buffer);
    return 0;
}


#if 0
        case AES_MODE_CFB1:
//int32_t csi_aes_cfb1_crypto(aes_handle_t handle, void *context, void *in, void *out,  uint32_t len, uint8_t iv[16]);

//            ret = csi_aes_cfb1_crypto(hd,NULL,ciphertext_ecb[td.key_len_bits],buffer,sizeof(ciphertext_cfb1[td.key_len_bits]),iv[16]);
            TEST_CASE_ASSERT_QUIT(ret == 0,"aes mode:cfb1 failed.\n");

//            ret = memcmp(ciphertext_cfb1[td.key_len_bits],buffer,16);
            TEST_CASE_ASSERT_QUIT(ret == 0,"aes decrypt failed.\n");
            break;
        case AES_MODE_CFB8:
//int32_t csi_aes_cfb8_crypto(aes_handle_t handle, void *context, void *in, void *out, uint32_t len, uint8_t iv[16]);

//            ret = csi_aes_cfb8_crypto(hd,NULL,ciphertext_ecb[td.key_len_bits],buffer,sizeof(ciphertext_cfb8[td.key_len_bits]),iv[16]);
            TEST_CASE_ASSERT_QUIT(ret == 0,"aes mode:cfb8 failed.\n");

//            ret = memcmp(ciphertext_cfb8[td.key_len_bits],buffer,16);
            TEST_CASE_ASSERT_QUIT(ret == 0,"aes decrypt failed.\n");
            break;
        case AES_MODE_CFB128:
//int32_t csi_aes_cfb128_crypto(aes_handle_t handle, void *context, void *in, void *out, uint32_t len, uint8_t iv[16], uint32_t *num);

//            ret = csi_aes_cfb128_crypto(hd,NULL,ciphertext_cfb128[td.key_len_bits],buffer,sizeof(ciphertext_cfb128[td.key_len_bits]),iv[16],&num);
            TEST_CASE_ASSERT_QUIT(ret == 0,"aes mode:cfb128 failed.\n");

//            ret = memcmp(ciphertext_cfb128[td.key_len_bits],buffer,16);
            TEST_CASE_ASSERT_QUIT(ret == 0,"aes decrypt failed.\n");
            break;
        case AES_MODE_OFB:
//int32_t csi_aes_ofb_crypto(aes_handle_t handle, void *context, void *in, void *out, uint32_t len, uint8_t iv[16], uint32_t *num);

//            ret = csi_aes_ofb_crypto(hd,NULL,ciphertext_ofb[td.key_len_bits],buffer,sizeof(ciphertext_ofb[td.key_len_bits]),iv[16],&num);
            TEST_CASE_ASSERT_QUIT(ret == 0,"aes mode:ofb failed.\n");

//            ret = memcmp(ciphertext_ofb[td.key_len_bits],buffer,16);
            TEST_CASE_ASSERT_QUIT(ret == 0,"aes decrypt failed.\n");
            break;
        case AES_MODE_CTR:
//int32_t csi_aes_ctr_crypto(aes_handle_t handle, void *context, void *in, void *out, uint32_t len, uint8_t nonce_counter[16], uint8_t stream_block[16], uint32_t *num);

//            ret = csi_aes_ctr_crypto(hd,NULL,ciphertext_ctr[td.key_len_bits],buffer,sizeof(ciphertext_ctr[td.key_len_bits]),iv[16]);//para error
            TEST_CASE_ASSERT_QUIT(ret == 0,"aes mode:ctr failed.\n");

//            ret = memcmp(ciphertext_ctr[td.key_len_bits],buffer,16);
            TEST_CASE_ASSERT_QUIT(ret == 0,"aes decrypt failed.\n");
            break;
#endif
