#include <string.h>
#include <stdio.h>
#include "aos/kernel.h"
#include "asm/barrier.h"
// #include "atoi.h"
#include "test_sdk_auth.h"
#include "aos/cli.h"
#include "sec_crypto_errcode.h"
#include "sec_crypto_base64.h"
#include "key_mgr.h"
#include "kp_sample.h"
#include "sec_crypto_sha.h"
#include "sec_device.h"

#define CHECK_RETURN(ret)                                                      \
        do {                                                                   \
                if (ret != 0) {                                                \
                        while (1)                                              \
                                ;                                              \
                        return -1;                                             \
                }                                                              \
        } while (0);

static int base64_test(void)
{
	char data[] = "123456789abcdef0";
	char data_en_ref[] = "MTIzNDU2Nzg5YWJjZGVmMA==";
	size_t buf_size;
	char *tmp_buf;
	uint32_t ret;

	buf_size = sc_base64_enc_len(sizeof(data));
	tmp_buf = malloc(buf_size);
	if (!tmp_buf) {
		printf(" Malloc buffer failed.\r\n");
		return -1;
	}

	ret = sc_base64_enc(data, sizeof(data) - 1, tmp_buf, &buf_size);
	if (ret != SC_OK) {
		printf("base64 encoding failed[0x%x] \r\n", ret);
		return ret;
	}

	for (int i=0; i < sizeof(data_en_ref); i++) {
		printf("%c, %c @ %d\r\n", tmp_buf[i], data_en_ref[i], i);
	}
	ret = memcmp(tmp_buf, data_en_ref, sizeof(data_en_ref) - 1);
	if (ret) {
		printf("data compare failed after encoding [0x%x] \r\n", ret);
		return -2;
	}

	ret = sc_base64_dec(tmp_buf, buf_size - 1, tmp_buf, &buf_size);
	if (ret != SC_OK) {
		printf("base64 decoding failed[0x%x] \r\n", ret);
		return ret;
	}

	ret = memcmp(data, tmp_buf, sizeof(data) - 1);
	if (ret) {
		printf("data compare failed after decoding [0x%x] \r\n", ret);
		return -2;
	}

	return 0;
}

static uint32_t sha256_process(uint8_t *data_in, size_t size, uint8_t *sha_out)
{
    uint32_t ret = 0;
    sc_sha_t sha;
	sc_sha_context_t context;
    uint32_t out_size = 0;        ///< bytes of calculation result

	if ((!data_in) || (!sha_out))
		return SC_PARAM_INV;

    /* STEP 1: init sha */
    ret = sc_sha_init(&sha, 0);
	if (ret != SC_OK)
		return ret;

    /* STEP 2: config sha-1*/
    ret = sc_sha_start(&sha, &context, SC_SHA_MODE_256);
	if (ret != SC_OK) {
		goto __uninit;
	}

    /* STEP 3: first encrypt characters(abc) */
    ret = sc_sha_update(&sha, &context, data_in, size);
	if (ret != SC_OK) {
		goto __uninit;
	}

    /* STEP 4: finish encrypt and output result */
    ret = sc_sha_finish(&sha, &context, sha_out, &out_size);
	if (ret != SC_OK) {
		goto __uninit;
	}
	printf("out_size: 0x%x \r\n", out_size);
	for(uint32_t i = 0; i < out_size; i++) {
		printf("0x%x @%d\r\n", sha_out[i], i);
	}

__uninit:
    /* STEP 5: uninit sha */
	sc_sha_uninit(&sha);

    return ret;
}
static int kp_test(void)
{
	uint8_t *kp_aft_dec;
	size_t buf_size;
	key_handle pub_key;
	key_handle sad;
	uint32_t key_size;
	uint8_t *data_ref;
	uint32_t ret;
#if 0
	/* Get kp buffer size after decoding */
	kp_aft_dec = malloc(sizeof(kp_src));
	if (!kp_aft_dec) {
		printf(" Malloc buffer failed.\r\n");
		return -1;
	}

	/* Decoding KP data */
	ret = sc_base64_dec(kp_src, sizeof(kp_src), kp_aft_dec, &buf_size);
	if (ret != SC_OK) {
		printf("base64 decoding failed[0x%x] \r\n", ret);
		return ret;
	}
#else
	kp_aft_dec = kp_encode_aes128;
	buf_size = sizeof(kp_encode_aes128);
#endif
	ret = km_init();
	if (ret != SC_OK) {
		printf("km_init failed[0x%x] \r\n", ret);
		return ret;
	}
	/* Update KP info */
	ret = km_update_kp(kp_aft_dec, buf_size);
	if (ret != SC_OK) {
		printf("update kp failed[0x%x] \r\n", ret);
		goto __exit;
	}
	/* Get public key */
	ret = km_get_key(KEY_ID_SDK_PUBKEY, &pub_key, &key_size);;
	if (ret != SC_OK) {
		printf("Get pub_key failed[0x%x] \r\n", ret);
		goto __exit;
	}

	data_ref = (uint8_t *)pub_key;
	ret = memcmp(data_ref, sdk_public_key, key_size);
	if (ret) {
		printf("pub_key check failed[0x%x] \r\n", ret);
		goto __exit;
	}

	/* Get and check sad */
	ret = km_get_key(KEY_ID_SAD, &sad, &key_size);;
	if (ret != SC_OK) {
		printf("Get pub_key failed[0x%x] \r\n", ret);
		goto __exit;
	}

	data_ref = (uint8_t *)pub_key;
	ret = memcmp(test_sad, (void *)sad, key_size);
	if (ret) {
		printf("pub_key check failed[0x%x] \r\n", ret);
		goto __exit;
	}

	/* caculate the hash value of public key */
	uint8_t hash[32];
	ret = sha256_process(data_ref, key_size, hash);
	if (ret) {
		printf("caculate the hash of pub_key failed[0x%x] \r\n", ret);
		goto __exit;
	}

	ret = memcmp(hash, pubkey_hash, sizeof(hash));
	if (ret) {
		printf("pub_key hash check failed[0x%x] \r\n", ret);
		goto __exit;
	}

	/* Autheticate sdk */
	sec_device_info_t sec_dev;
	
   	sec_dev.pub_key = data_ref;
    sec_dev.pub_key_size = 0x100;
    sec_dev.pubkey_hash = hash;
    sec_dev.pubkey_hash_size = sizeof(hash);
    sec_dev.huk = test_huk;
    sec_dev.huk_size = sizeof(test_huk) - 1;	/*ignore the last charactor '\0' */
//    sec_dev.chip_id = ;
//    sec_dev.chip_id_size;
    sec_dev.sap = 0x000001E0;
    sec_dev.sad = test_sad;
    sec_dev.sad_size = sizeof(test_sad);
	ret = sc_device_auth_probe(&sec_dev);
	if (ret) {
		printf("sdk autheticating failed[0x%x] \r\n", ret);
		goto __exit;
	}

	/* Test secure storage data */
	sc_dev_storage_t sec_storage;
	 sec_storage.idx = 0;
	ret = sc_device_storage_init(&sec_storage);
	if (ret) {
		printf("secure device init failed[0x%x] \r\n", ret);
		goto __exit;
	}
	sc_dev_data_info_t data_info;
	unsigned char test_data[] = 
				{0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x00,
				 0x12, 0x23, 0x34, 0x45, 0x56, 0x67, 0x78, 0x89, 0x9a, 0xab, 0xbc, 0xcd, 0xde, 0xef, 0xf0, 0x01};
	unsigned char test_data_enc[] = 
				{0x27, 0xD2, 0xEB, 0xA3, 0xE2, 0x2B, 0xCB, 0x22, 0xC7, 0xBA, 0x09, 0x1C, 0x1C, 0xDF, 0x24, 0xAA,  
				0xD3, 0xF4, 0x64, 0x21, 0x62, 0x1E, 0xAF, 0x05, 0x21, 0x88, 0x44, 0x32, 0x88, 0x2B, 0xCC, 0xDD,};
	unsigned char test_tmp_buf[32];
	data_info.data_in = test_data;
	data_info.data_out = test_tmp_buf;
	data_info.data_size = sizeof(test_data);
	ret = sc_device_storage_encrypt(&sec_storage, &data_info);
	if (ret) {
		printf("secure device encrypt failed[0x%x] \r\n", ret);
		goto __sec_dev_exit;
	}
	ret = memcmp(test_tmp_buf, test_data_enc, data_info.data_size);
	if (ret) {
		printf("Secure data encryption check failed[0x%x] \r\n", ret);
		goto __sec_dev_exit;
	} else {
		printf("Secure data encryption check success\r\n");
	}
	
	data_info.data_in = test_tmp_buf;
	data_info.data_out = test_tmp_buf;
	data_info.data_size = sizeof(test_data);
	ret = sc_device_storage_decrypt(&sec_storage, &data_info);
	if (ret) {
		printf("secure device encrypt failed[0x%x] \r\n", ret);
		goto __sec_dev_exit;
	}
	ret = memcmp(test_tmp_buf, test_data, data_info.data_size);
	if (ret) {
		printf("Secure data decryption check failed[0x%x] \r\n", ret);
		goto __sec_dev_exit;
	} else {
		printf("Secure data decryption check success\r\n");
	}

__sec_dev_exit:
	sc_device_storage_uninit(&sec_storage);

__exit:
	km_uninit();

	return ret;
}

void sdk_auth_test(int32_t argc, char **argv)
{
	if(argc == 2) {
		if(strcmp(argv[1], "base64") == 0) {
			if (!base64_test()) {
				printf("=============== base64 test success \r\n");
			} else {
				printf("=============== base64 test failed \r\n");
			}
		} else if(strcmp(argv[1], "kp") == 0) {
			if (!kp_test()) {
				printf("=============== kp_test success \r\n");
			} else {
				printf("=============== kp_test failed \r\n");
			}
		}
	}
}

ALIOS_CLI_CMD_REGISTER(sdk_auth_test, sdk_auth, sdk_auth);

