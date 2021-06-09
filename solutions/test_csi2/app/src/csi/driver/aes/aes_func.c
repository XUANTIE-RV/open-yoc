/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include "aes_test.h"

uint8_t iv_value[AES_KEY_LEN];
uint8_t key_value[AES_DATA_LEN];
uint8_t aes_iv_value[AES_DATA_LEN];
uint8_t aes_key_buf[AES_KEY_LEN];
uint8_t aes_iv_buf[AES_IV_LEN];
char enc_data[AES_DATA_LEN];
uint8_t nonce_counter[AES_DATA_LEN];
uint8_t stream_block[AES_DATA_LEN];
char src_data[AES_DATA_LEN];
char aes_buf[AES_KEY_LEN];
uint8_t aes_src_data[AES_DATA_LEN];
uint8_t aes_dec_data[AES_DATA_LEN];
uint8_t aes_enc_data[AES_DATA_LEN];
int key_len_bit = 0;
int key_len = 0;
char aes_buf[AES_KEY_LEN];


int set_aes_key(char *args)
{
    sscanf(args, "%[^,],%d", aes_buf, &key_len);
    hex_to_array(aes_buf,  key_len, aes_key_buf);

    switch (key_len) {
        case 16:
            key_len_bit =  AES_KEY_LEN_BITS_128;
            break;

        case 24:
            key_len_bit =  AES_KEY_LEN_BITS_192;
            break;

        case 32:
            key_len_bit =  AES_KEY_LEN_BITS_256;
            break;

        default:
            TEST_CASE_TIPS("key_len %u error", key_len);
            return -1;
    }

    TEST_PRINT_HEX("aes_key_buf:", aes_key_buf, key_len);
    TEST_CASE_TIPS("set AES key successfully");
    return 0;
}

int set_aes_iv(char *args)
{
    sscanf(args, "%[,]", aes_buf);
    hex_to_array(aes_buf,  16, aes_iv_buf);

    TEST_PRINT_HEX("aes_iv_buf:", aes_iv_buf, 16);
    TEST_CASE_TIPS("set AES iv key successfully");
    return 0;
}


int aes_ecb_encrypt(char *args)
{
    uint32_t ret = 0;
    csi_aes_t aes;
    uint32_t idx;
    int data_len = 0;

    sscanf(args, "%d,%[^,],%d", &idx, src_data, &data_len);
    hex_to_array(src_data,  data_len, aes_src_data);
    TEST_CASE_TIPS("data_len is %d\n", data_len);

    ret = csi_aes_init(&aes, idx);
    TEST_CASE_ASSERT_QUIT(ret == CSI_OK, "csi_aes_init failed with idx: %d", idx);

    ret = csi_aes_set_encrypt_key(&aes, aes_key_buf, key_len_bit);
    TEST_CASE_ASSERT(ret == CSI_OK, "csi_aes_set_encrypt_key should return CSI_OK, but return %d", ret);

    ret = csi_aes_ecb_encrypt(&aes, aes_src_data, aes_enc_data, data_len);
    TEST_CASE_ASSERT_QUIT(ret == CSI_OK, "csi_aes_ecb_encrypt should return CSI_OK, but return %d", ret);

    TEST_PRINT_HEX("ENCRYPTED DATA:", aes_enc_data, data_len);

    csi_aes_uninit(&aes);
    return 0;
}


int aes_ecb_decrypt(char *args)
{
    uint32_t ret = 0;
    csi_aes_t aes;
    uint32_t idx;

    int data_len;

    sscanf(args, "%d,%[^,],%d", &idx, enc_data, &data_len);
    hex_to_array(enc_data,  data_len, aes_src_data);

    ret = csi_aes_init(&aes, idx);
    TEST_CASE_ASSERT_QUIT(ret == CSI_OK, "csi_aes_init failed with idx: %d", idx);

    ret = csi_aes_set_decrypt_key(&aes, aes_key_buf, key_len_bit);
    TEST_CASE_ASSERT(ret == CSI_OK, "csi_aes_set_decrypt_key should return CSI_OK, but return %d", ret);

    ret = csi_aes_ecb_decrypt(&aes, aes_src_data, aes_dec_data, data_len);
    TEST_CASE_ASSERT_QUIT(ret == CSI_OK, "csi_aes_ecb_decrypt should return CSI_OK, but return %d", ret);

    TEST_PRINT_HEX("DENCRYPTED DATA:", aes_dec_data, data_len);

    csi_aes_uninit(&aes);
    return 0;
}

int aes_cbc_encrypt(char *args)
{
    uint32_t ret = 0;
    csi_aes_t aes;
    uint32_t idx;

    int data_len = 0;

    sscanf(args, "%d,%[^,],%d", &idx, src_data, &data_len);
    hex_to_array(src_data,  data_len, aes_src_data);

    ret = csi_aes_init(&aes, idx);
    TEST_CASE_ASSERT_QUIT(ret == CSI_OK, "csi_aes_init failed with idx: %d", idx);

    ret = csi_aes_set_encrypt_key(&aes, aes_key_buf, key_len_bit);
    TEST_CASE_ASSERT(ret == CSI_OK, "csi_aes_set_encrypt_key should return CSI_OK, but return %d", ret);

    ret = csi_aes_cbc_encrypt(&aes, aes_src_data, aes_enc_data, data_len, aes_iv_buf);
    TEST_CASE_ASSERT_QUIT(ret == CSI_OK, "csi_aes_cbc_encrypt should return CSI_OK, but return %d", ret);

    TEST_PRINT_HEX("ENCRYPTED DATA:", aes_enc_data, data_len);

    csi_aes_uninit(&aes);
    return 0;
}

int aes_cbc_decrypt(char *args)
{
    uint32_t ret = 0;
    csi_aes_t aes;
    uint32_t idx;

    int data_len = 0;

    sscanf(args, "%d,%[^,],%d", &idx, src_data, &data_len);
    hex_to_array(src_data,  data_len, aes_src_data);

    ret = csi_aes_init(&aes, idx);
    TEST_CASE_ASSERT_QUIT(ret == CSI_OK, "csi_aes_init failed with idx: %d", idx);

    ret = csi_aes_set_decrypt_key(&aes, aes_key_buf, key_len_bit);
    TEST_CASE_ASSERT(ret == CSI_OK, "csi_aes_set_decrypt_key should return CSI_OK, but return %d", ret);

    ret = csi_aes_cbc_decrypt(&aes, aes_src_data, aes_dec_data, data_len, aes_iv_buf);
    TEST_CASE_ASSERT_QUIT(ret == CSI_OK, "csi_aes_cbc_decrypt should return CSI_OK, but return %d", ret);

    TEST_PRINT_HEX("DECRYPTED DATA:", aes_dec_data, data_len);

    csi_aes_uninit(&aes);
    return 0;
}

int aes_cfb128_encrypt(char *args)
{
    uint32_t ret = 0;
    csi_aes_t aes;
    uint32_t idx;
    uint32_t num;

    int data_len = 0;

    sscanf(args, "%d,%[^,],%d,%d", &idx, src_data, &data_len, &num);
    hex_to_array(src_data,  data_len, aes_src_data);

    ret = csi_aes_init(&aes, idx);
    TEST_CASE_ASSERT_QUIT(ret == CSI_OK, "csi_aes_init failed with idx: %d", idx);

    ret = csi_aes_set_encrypt_key(&aes, aes_key_buf, key_len_bit);
    TEST_CASE_ASSERT(ret == CSI_OK, "csi_aes_set_encrypt_key should return CSI_OK, but return %d", ret);

    ret = csi_aes_cfb128_encrypt(&aes, aes_src_data, aes_enc_data, data_len, aes_iv_buf, &num);
    TEST_CASE_ASSERT_QUIT(ret == CSI_OK, "csi_aes_cfb128_encrypt should return CSI_OK, but return %d", ret);

    TEST_PRINT_HEX("ENCRYPTED DATA:", aes_enc_data, data_len);

    csi_aes_uninit(&aes);
    return 0;
}

int aes_cfb128_decrypt(char *args)
{
    uint32_t ret = 0;
    csi_aes_t aes;
    uint32_t idx;
    uint32_t num;

    int data_len = 0;

    sscanf(args, "%d,%[^,],%d,%d", &idx, src_data, &data_len, &num);
    hex_to_array(src_data,  data_len, aes_src_data);

    ret = csi_aes_init(&aes, idx);
    TEST_CASE_ASSERT_QUIT(ret == CSI_OK, "csi_aes_init failed with idx: %d", idx);

    ret = csi_aes_set_decrypt_key(&aes, aes_key_buf, key_len_bit);
    TEST_CASE_ASSERT(ret == CSI_OK, "csi_aes_set_decrypt_key should return CSI_OK, but return %d", ret);

    ret = csi_aes_cfb128_decrypt(&aes, aes_src_data, aes_dec_data, data_len, aes_iv_buf, &num);
    TEST_CASE_ASSERT_QUIT(ret == CSI_OK, "csi_aes_cfb128_decrypt should return CSI_OK, but return %d", ret);

    TEST_PRINT_HEX("DECRYPTED DATA:", aes_dec_data, data_len);

    csi_aes_uninit(&aes);
    return 0;
}

int aes_ctr_encrypt(char *args)
{
    uint32_t ret = 0;
    csi_aes_t aes;
    uint32_t idx;
    uint32_t num;

    int data_len = 0;

    sscanf(args, "%d,%[^,],%d,%d", &idx, src_data, &data_len, &num);
    hex_to_array(src_data,  data_len, aes_src_data);

    ret = csi_aes_init(&aes, idx);
    TEST_CASE_ASSERT_QUIT(ret == CSI_OK, "csi_aes_init failed with idx: %d", idx);

    ret = csi_aes_set_encrypt_key(&aes, aes_key_buf, key_len_bit);
    TEST_CASE_ASSERT(ret == CSI_OK, "csi_aes_set_encrypt_key should return CSI_OK, but return %d", ret);

    ret = csi_aes_ctr_encrypt(&aes, aes_src_data, aes_enc_data, data_len, nonce_counter, stream_block, aes_iv_buf, &num);
    TEST_CASE_ASSERT_QUIT(ret == CSI_OK, "csi_aes_crt_encrypt should return CSI_OK, but return %d", ret);

    TEST_PRINT_HEX("ENCRYPTED DATA:", aes_enc_data, data_len);

    csi_aes_uninit(&aes);
    return 0;
}

int aes_ctr_decrypt(char *args)
{
    uint32_t ret = 0;
    csi_aes_t aes;
    uint32_t idx;
    uint32_t num;
    int data_len = 0;

    sscanf(args, "%d,%[^,],%d,%d", &idx, src_data, &data_len, &num);
    hex_to_array(src_data,  data_len, aes_src_data);

    ret = csi_aes_init(&aes, idx);
    TEST_CASE_ASSERT_QUIT(ret == CSI_OK, "csi_aes_init failed with idx: %d", idx);

    ret = csi_aes_set_decrypt_key(&aes, aes_key_buf, key_len_bit);
    TEST_CASE_ASSERT(ret == CSI_OK, "csi_aes_set_encrypt_key should return CSI_OK, but return %d", ret);

    ret = csi_aes_ctr_decrypt(&aes, aes_src_data, aes_dec_data, data_len, nonce_counter, stream_block, aes_iv_buf, &num);
    TEST_CASE_ASSERT_QUIT(ret == CSI_OK, "csi_aes_crt_decrypt should return CSI_OK, but return %d", ret);

    TEST_PRINT_HEX("DECRYPTED DATA:", aes_dec_data, data_len);

    csi_aes_uninit(&aes);
    return 0;
}


int aes_ecb_encrypt_performance(char *args)
{
    uint32_t ret = 0;
    csi_aes_t aes;
    uint32_t idx;
    int data_len = 0;
    int number_of_loop;
    int i ;
    int time_ms;
    float performance;

    sscanf(args, "%d,%d,%d,%d", &idx, &key_len, &data_len, &number_of_loop);

    switch (key_len) {
        case 16:
            key_len_bit =  AES_KEY_LEN_BITS_128;
            break;

        case 24:
            key_len_bit =  AES_KEY_LEN_BITS_192;
            break;

        case 32:
            key_len_bit =  AES_KEY_LEN_BITS_256;
            break;

        default:
            TEST_CASE_TIPS("key_len %u error", key_len);
            return -1;
    }

    ret = csi_aes_init(&aes, idx);
    TEST_CASE_ASSERT_QUIT(ret == CSI_OK, "csi_aes_init failed with idx: %d", idx);

    tst_timer_restart();

    for (i = 1; i <= number_of_loop; i++) {
        generate_rand_array2(aes_src_data, 255, data_len);
        generate_rand_array2(aes_key_buf, 255, key_len);

        ret = csi_aes_set_encrypt_key(&aes, aes_key_buf, key_len_bit);
        TEST_CASE_ASSERT(ret == CSI_OK, "csi_aes_set_encrypt_key should return CSI_OK, but return %d", ret);

        ret = csi_aes_ecb_encrypt(&aes, aes_src_data, aes_enc_data, data_len);

        if (ret != 0) {
            TEST_PRINT_HEX("KEY:", aes_key_buf, key_len_bit);
            TEST_PRINT_HEX("SRC DATA:", aes_src_data, data_len);
            TEST_PRINT_HEX("ENC DATA:", aes_enc_data, data_len);
            TEST_CASE_ASSERT(ret == 0, "Aes ecb encrypt failed!\n");
        }
    }

    time_ms = tst_timer_get_interval();
    performance = (1.0 * number_of_loop) / time_ms;
    TEST_CASE_TIPS("Change data every loop:test AES ecb encrypt performance value is %f times/ms", performance);

    memset(aes_src_data, 0x00, data_len);
    memset(aes_key_buf, 0x00, key_len);

    tst_timer_restart();

    generate_rand_array2(aes_src_data, 255, data_len);
    generate_rand_array2(aes_key_buf, 255, key_len);
    ret = csi_aes_set_encrypt_key(&aes, aes_key_buf, key_len_bit);
    TEST_CASE_ASSERT(ret == CSI_OK, "csi_aes_set_encrypt_key should return CSI_OK, but return %d", ret);

    for (i = 1; i <= number_of_loop; i++) {
        ret = csi_aes_ecb_encrypt(&aes, aes_src_data, aes_enc_data, data_len);

        if (ret != 0) {
            TEST_PRINT_HEX("KEY:", aes_key_buf, key_len_bit);
            TEST_PRINT_HEX("SRC DATA:", aes_src_data, data_len);
            TEST_PRINT_HEX("ENC DATA:", aes_enc_data, data_len);
            TEST_CASE_ASSERT(ret == 0, "Aes ecb encrypt failed!\n");
        }
    }

    time_ms = tst_timer_get_interval();
    performance = (1.0 * number_of_loop) / time_ms;
    TEST_CASE_TIPS("Keep data constant:test AES ecb encrypt performance value is %f times/ms", performance);

    csi_aes_uninit(&aes);
    return 0;
}


int aes_ecb_decrypt_performance(char *args)
{
    uint32_t ret = 0;
    csi_aes_t aes;
    uint32_t idx;
    int data_len = 0;
    int number_of_loop;
    int i ;
    int time_ms;
    float performance;

    sscanf(args, "%d,%d,%d,%d", &idx, &key_len, &data_len, &number_of_loop);

    switch (key_len) {
        case 16:
            key_len_bit =  AES_KEY_LEN_BITS_128;
            break;

        case 24:
            key_len_bit =  AES_KEY_LEN_BITS_192;
            break;

        case 32:
            key_len_bit =  AES_KEY_LEN_BITS_256;
            break;

        default:
            TEST_CASE_TIPS("key_len %u error", key_len);
            return -1;
    }

    ret = csi_aes_init(&aes, idx);
    TEST_CASE_ASSERT_QUIT(ret == CSI_OK, "csi_aes_init failed with idx: %d", idx);

    tst_timer_restart();

    for (i = 1; i <= number_of_loop; i++) {
        generate_rand_array2(aes_src_data, 255, data_len);
        generate_rand_array2(aes_key_buf, 255, key_len);

        ret = csi_aes_set_decrypt_key(&aes, aes_key_buf, key_len_bit);
        TEST_CASE_ASSERT(ret == CSI_OK, "csi_aes_set_encrypt_key should return CSI_OK, but return %d", ret);

        ret = csi_aes_ecb_decrypt(&aes, aes_src_data, aes_dec_data, data_len);

        if (ret != 0) {
            TEST_PRINT_HEX("KEY:", aes_key_buf, key_len_bit);
            TEST_PRINT_HEX("SRC DATA:", aes_src_data, data_len);
            TEST_PRINT_HEX("DEC DATA:", aes_dec_data, data_len);
            TEST_CASE_ASSERT(ret == 0, "Aes ecb decrypt failed!\n");
        }
    }

    time_ms = tst_timer_get_interval();
    performance = (1.0 * number_of_loop) / time_ms;
    TEST_CASE_TIPS("Change data every loop:test AES ecb decrypt Performance value is %f times/ms", performance);

    tst_timer_restart();
    generate_rand_array2(aes_src_data, 255, data_len);
    generate_rand_array2(aes_key_buf, 255, key_len);

    for (i = 1; i <= number_of_loop; i++) {
        ret = csi_aes_set_decrypt_key(&aes, aes_key_buf, key_len_bit);
        TEST_CASE_ASSERT(ret == CSI_OK, "csi_aes_set_encrypt_key should return CSI_OK, but return %d", ret);

        ret = csi_aes_ecb_decrypt(&aes, aes_src_data, aes_dec_data, data_len);

        if (ret != 0) {
            TEST_PRINT_HEX("KEY:", aes_key_buf, key_len_bit);
            TEST_PRINT_HEX("SRC DATA:", aes_src_data, data_len);
            TEST_PRINT_HEX("DEC DATA:", aes_dec_data, data_len);
            TEST_CASE_ASSERT(ret == 0, "Aes ecb decrypt failed!\n");
        }
    }

    time_ms = tst_timer_get_interval();
    performance = (1.0 * number_of_loop) / time_ms;
    TEST_CASE_TIPS("Keep data constant:test AES ecb decrypt Performance value is %f times/ms", performance);

    csi_aes_uninit(&aes);
    return 0;
}

int aes_cbc_encrypt_performance(char *args)
{
    uint32_t ret = 0;
    csi_aes_t aes;
    uint32_t idx;
    int data_len = 0;
    int number_of_loop;
    int i ;
    int time_ms;
    float performance;

    sscanf(args, "%d,%d,%d,%d", &idx, &key_len, &data_len, &number_of_loop);

    switch (key_len) {
        case 16:
            key_len_bit =  AES_KEY_LEN_BITS_128;
            break;

        case 24:
            key_len_bit =  AES_KEY_LEN_BITS_192;
            break;

        case 32:
            key_len_bit =  AES_KEY_LEN_BITS_256;
            break;

        default:
            TEST_CASE_TIPS("key_len %u error", key_len);
            return -1;
    }

    ret = csi_aes_init(&aes, idx);
    TEST_CASE_ASSERT_QUIT(ret == CSI_OK, "csi_aes_init failed with idx: %d", idx);

    tst_timer_restart();

    for (i = 1; i <= number_of_loop; i++) {
        generate_rand_array2(aes_src_data, 255, data_len);
        generate_rand_array2(aes_key_buf, 255, key_len);
        generate_rand_array2(aes_iv_buf, 255, 16);

        ret = csi_aes_set_encrypt_key(&aes, aes_key_buf, key_len_bit);
        TEST_CASE_ASSERT(ret == CSI_OK, "csi_aes_set_encrypt_key should return CSI_OK, but return %d", ret);
        ret = csi_aes_cbc_encrypt(&aes, aes_src_data, aes_enc_data, data_len, aes_iv_buf);

        if (ret != 0) {
            TEST_PRINT_HEX("KEY:", aes_key_buf, key_len_bit);
            TEST_PRINT_HEX("IV:", aes_iv_buf, 16);
            TEST_PRINT_HEX("SRC DATA:", aes_src_data, data_len);
            TEST_PRINT_HEX("ENC DATA:", aes_enc_data, data_len);
            TEST_CASE_ASSERT(ret == 0, "Aes ecb encrypt failed!\n");
        }
    }

    time_ms = tst_timer_get_interval();
    performance = (1.0 * number_of_loop) / time_ms;
    TEST_CASE_TIPS("Change data every loop:test AES cbc encrypt Performance value is %f times/ms", performance);


    tst_timer_restart();
    generate_rand_array2(aes_src_data, 255, data_len);
    generate_rand_array2(aes_key_buf, 255, key_len);
    generate_rand_array2(aes_iv_buf, 255, 16);

    for (i = 1; i <= number_of_loop; i++) {
        memcpy(iv_value, aes_iv_buf, 16);
        ret = csi_aes_set_encrypt_key(&aes, aes_key_buf, key_len_bit);
        TEST_CASE_ASSERT(ret == CSI_OK, "csi_aes_set_encrypt_key should return CSI_OK, but return %d", ret);
        ret = csi_aes_cbc_encrypt(&aes, aes_src_data, aes_enc_data, data_len, iv_value);

        if (ret != 0) {
            TEST_PRINT_HEX("KEY:", aes_key_buf, key_len_bit);
            TEST_PRINT_HEX("IV:", iv_value, 16);
            TEST_PRINT_HEX("SRC DATA:", aes_src_data, data_len);
            TEST_PRINT_HEX("ENC DATA:", aes_enc_data, data_len);
            TEST_CASE_ASSERT(ret == 0, "Aes ecb encrypt failed!\n");
        }
    }

    time_ms = tst_timer_get_interval();
    performance = (1.0 * number_of_loop) / time_ms;
    TEST_CASE_TIPS("Keep data constant:test AES cbc encrypt Performance value is %f times/ms", performance);

    csi_aes_uninit(&aes);
    return 0;
}

int aes_cbc_decrypt_performance(char *args)
{
    uint32_t ret = 0;
    csi_aes_t aes;
    uint32_t idx;
    int data_len = 0;
    int number_of_loop;
    int i ;
    int time_ms;
    float performance;

    sscanf(args, "%d,%d,%d,%d", &idx, &key_len, &data_len, &number_of_loop);

    switch (key_len) {
        case 16:
            key_len_bit =  AES_KEY_LEN_BITS_128;
            break;

        case 24:
            key_len_bit =  AES_KEY_LEN_BITS_192;
            break;

        case 32:
            key_len_bit =  AES_KEY_LEN_BITS_256;
            break;

        default:
            TEST_CASE_TIPS("key_len %u error", key_len);
            return -1;
    }

    ret = csi_aes_init(&aes, idx);
    TEST_CASE_ASSERT_QUIT(ret == CSI_OK, "csi_aes_init failed with idx: %d", idx);

    tst_timer_restart();

    for (i = 1; i <= number_of_loop; i++) {
        generate_rand_array2(aes_src_data, 255, data_len);
        generate_rand_array2(aes_key_buf, 255, key_len);
        generate_rand_array2(aes_iv_buf, 255, 16);

        ret = csi_aes_set_encrypt_key(&aes, aes_key_buf, key_len_bit);
        TEST_CASE_ASSERT(ret == CSI_OK, "csi_aes_set_encrypt_key should return CSI_OK, but return %d", ret);

        ret = csi_aes_cbc_decrypt(&aes, aes_src_data, aes_dec_data, data_len, iv_value);

        if (ret != 0) {
            TEST_PRINT_HEX("KEY:", aes_key_buf, key_len_bit);
            TEST_PRINT_HEX("IV:", iv_value, 16);
            TEST_PRINT_HEX("SRC DATA:", aes_src_data, data_len);
            TEST_PRINT_HEX("DEC DATA:", aes_dec_data, data_len);
            TEST_CASE_ASSERT(ret == 0, "Aes ecb decrypt failed!\n");
        }
    }

    time_ms = tst_timer_get_interval();
    performance = (1.0 * number_of_loop) / time_ms;
    TEST_CASE_TIPS("Change data every loop:test AES cbc decrypt Performance value is %f times/ms", performance);

    tst_timer_restart();
    generate_rand_array2(aes_src_data, 255, data_len);
    generate_rand_array2(aes_key_buf, 255, key_len);
    generate_rand_array2(aes_iv_buf, 255, 16);

    for (i = 1; i <= number_of_loop; i++) {
        memcpy(iv_value, aes_iv_buf, 16);
        ret = csi_aes_set_encrypt_key(&aes, aes_key_buf, key_len_bit);
        TEST_CASE_ASSERT(ret == CSI_OK, "csi_aes_set_encrypt_key should return CSI_OK, but return %d", ret);

        ret = csi_aes_cbc_decrypt(&aes, aes_src_data, aes_dec_data, data_len, iv_value);

        if (ret != 0) {
            TEST_PRINT_HEX("KEY:", aes_key_buf, key_len_bit);
            TEST_PRINT_HEX("IV:", iv_value, 16);
            TEST_PRINT_HEX("SRC DATA:", aes_src_data, data_len);
            TEST_PRINT_HEX("DEC DATA:", aes_dec_data, data_len);
            TEST_CASE_ASSERT(ret == 0, "Aes ecb decrypt failed!\n");
        }
    }

    time_ms = tst_timer_get_interval();
    performance = (1.0 * number_of_loop) / time_ms;
    TEST_CASE_TIPS("Keep data constant:test AES cbc decrypt Performance value is %f times/ms", performance);

    csi_aes_uninit(&aes);
    return 0;
}


int aes_ecb_encrypt_decrypt_stability(char *args)
{
    uint32_t ret = 0;
    csi_aes_t aes;
    uint32_t idx;

    int data_len = 0;
    int number_of_loop;

    sscanf(args, "%d,%d,%d,%d", &idx, &key_len, &data_len, &number_of_loop);
    TEST_CASE_TIPS("data_len is %d\n", data_len);
    TEST_CASE_TIPS("key_len is %d\n", key_len);

    switch (key_len) {
        case 16:
            key_len_bit =  AES_KEY_LEN_BITS_128;
            break;

        case 24:
            key_len_bit =  AES_KEY_LEN_BITS_192;
            break;

        case 32:
            key_len_bit =  AES_KEY_LEN_BITS_256;
            break;

        default:
            TEST_CASE_TIPS("key_len %u error", key_len);
            return -1;
    }

    ret = csi_aes_init(&aes, idx);
    TEST_CASE_ASSERT_QUIT(ret == CSI_OK, "csi_aes_init failed with idx: %d", idx);

    for (int i = 1; i <= number_of_loop; i++) {
        generate_rand_array2(key_value, 255, key_len);
        generate_rand_array2(aes_src_data, 255, data_len);
        TEST_PRINT_HEX("KEY:", key_value, key_len);
        TEST_PRINT_HEX("SRC DATA:", aes_src_data, data_len);

        ret = csi_aes_set_encrypt_key(&aes, key_value, key_len_bit);
        TEST_CASE_ASSERT(ret == CSI_OK, "csi_aes_set_encrypt_key should return CSI_OK, but return %d", ret);

        ret = csi_aes_ecb_encrypt(&aes, aes_src_data, aes_enc_data, data_len);
        TEST_CASE_ASSERT(ret == CSI_OK, "csi_aes_ecb_encrypt should return CSI_OK, but return %d", ret);
        TEST_PRINT_HEX("ENCRYPTED DATA:", aes_enc_data, data_len);

        ret = csi_aes_ecb_decrypt(&aes, aes_enc_data, aes_dec_data, data_len);
        TEST_CASE_ASSERT(ret == CSI_OK, "csi_aes_ecb_decrypt should return CSI_OK, but return %d", ret);
        TEST_PRINT_HEX("DECRYPTED DATA:", aes_dec_data, data_len);

        ret = memcmp(aes_src_data, aes_dec_data, data_len);

        memset(aes_enc_data, 0x00, data_len);
        memset(aes_dec_data, 0x00, data_len);
    }

    csi_aes_uninit(&aes);
    return 0;
}

int aes_cbc_encrypt_decrypt_stability(char *args)
{
    uint32_t ret = 0;
    csi_aes_t aes;
    uint32_t idx;

    int data_len = 0;
    int key_len = 0;
    int number_of_loop;
    int i ;

    sscanf(args, "%d,%d,%d,%d", &idx, &key_len, &data_len, &number_of_loop);

    switch (key_len) {
        case 16:
            key_len_bit =  AES_KEY_LEN_BITS_128;
            break;

        case 24:
            key_len_bit =  AES_KEY_LEN_BITS_192;
            break;

        case 32:
            key_len_bit =  AES_KEY_LEN_BITS_256;
            break;

        default:
            TEST_CASE_TIPS("key_len set error");
            return -1;
    }

    ret = csi_aes_init(&aes, idx);
    TEST_CASE_ASSERT_QUIT(ret == CSI_OK, "csi_aes_init failed with idx: %d", idx);

    for (i = 1; i <= number_of_loop; i++) {
        generate_rand_array2(aes_iv_value, 255, 16);
        generate_rand_array2(key_value, 255, key_len);
        generate_rand_array2(aes_src_data, 255, data_len);
        TEST_PRINT_HEX("SRC DATA:", aes_src_data, data_len);
        TEST_PRINT_HEX("KEY:", key_value, key_len);
        TEST_PRINT_HEX("IV:", aes_iv_value, 16);

        ret = csi_aes_set_encrypt_key(&aes, key_value, key_len_bit);
        TEST_CASE_ASSERT(ret == CSI_OK, "csi_aes_set_encrypt_key should return CSI_OK, but return %d", ret);

        memcpy(iv_value, aes_iv_value, 16);

        ret = csi_aes_cbc_encrypt(&aes, aes_src_data, aes_enc_data, data_len, iv_value);
        TEST_CASE_ASSERT_QUIT(ret == CSI_OK, "AES encrypt error!\n");
        TEST_PRINT_HEX("ENCRYPTED DATA:", aes_enc_data, data_len);

        memcpy(iv_value, aes_iv_value, 16);

        ret = csi_aes_cbc_decrypt(&aes, aes_enc_data, aes_dec_data, data_len, iv_value);
        TEST_CASE_ASSERT_QUIT(ret == CSI_OK, "AES decrypt error!\n");
        TEST_PRINT_HEX("DECRYPTED DATA:", aes_dec_data, data_len);

        ret = memcmp(aes_src_data, aes_dec_data, data_len);
        TEST_CASE_ASSERT_QUIT(ret == 0, "AES encrypt_and_decrypt_function test failed!\n");

        memset(aes_src_data, 0x00, data_len);
        memset(aes_enc_data, 0x00, data_len);
        memset(aes_dec_data, 0x00, data_len);
        memset(key_value, 0x00, data_len);
        memset(aes_iv_value, 0x00, 16);
    }

    csi_aes_uninit(&aes);
    return 0;
}

