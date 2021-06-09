/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */


#include "rsa_test.h"

test_rsa_args_t rsa_test_param;
uint8_t rsa_temp[RSA_SOURCE_DATA_LEN];
uint8_t rsa_source_data[RSA_SOURCE_DATA_LEN];
uint8_t rsa_target_data[RSA_TARGET_DATA_LEN];
uint8_t rsa_enc_data[RSA_SOURCE_DATA_LEN];
uint8_t rsa_dec_data[RSA_TARGET_DATA_LEN];
uint8_t rsa_key_buf[RSA_KEY_LEN];
extern uint8_t rsa_d1024[128];
extern uint8_t rsa_e1024[128];
extern uint8_t rsa_n1024[128];
extern uint8_t rsa_d2048[256];
extern uint8_t rsa_e2048[256];
extern uint8_t rsa_n2048[256];

char rsa_received_data_from_host[RSA_RECV_HOST_DATA_LEN];
char rsa_key_data[RSA_KEY_LEN];

volatile uint32_t async_verify_flag = 0;
volatile uint32_t rsa_event_finish = 0;

int rsa_key_len_bit = 0;
int rsa_key_len = 0;
csi_rsa_context_t context;

static void rsa_test_cb(csi_rsa_t *rsa, csi_rsa_event_t event, void *arg)
{
    switch (event) {
        case RSA_EVENT_COMPLETE:
            rsa_event_finish = 0;
            break;

        case RSA_EVENT_VERIFY_SUCCESS:
            async_verify_flag = 0;
            break;

        case RSA_EVENT_VERIFY_FAILED:
            break;

        case RSA_EVENT_ERROR:
            break;

        default:
            break;
    }

}

static int get_rsa_test_params(char *args, csi_rsa_context_t *context){
    sscanf(args, "%d,%d,%d,%d,", &rsa_test_param.idx, \
                                    &rsa_test_param.key_bits, \
                                    &rsa_test_param.padding_type, \
                                    &rsa_test_param.hash_type 
                                    );

    context->key_bits = rsa_test_param.key_bits;
    context->padding_type =  rsa_test_param.padding_type; 
    
    switch (rsa_test_param.key_bits)
    {
    case RSA_KEY_BITS_1024:
        context->d = (void *) rsa_d1024;
        context->e =(void *)  rsa_e1024;
        context->n = (void *) rsa_n1024;
        rsa_test_param.data_len = 128;
        break;
    case RSA_KEY_BITS_2048:
        context->d = (void *) rsa_d2048;
        context->e = (void *) rsa_e2048;
        context->n =(void *)  rsa_n2048;
        rsa_test_param.data_len = 256;
        break;
    default:
        TEST_CASE_ASSERT_QUIT(1==0,"RSA test only support key length 1024 & 2048 now");
        return -1;
        break;
    }
    return 0;
}

int test_set_rsa_key(char *args)
{
    int key_type = 0;
    sscanf(args, "%d,%[^],%d", &key_type, rsa_key_data, &rsa_key_len);
    hex_to_array(rsa_key_data, rsa_key_len, rsa_key_buf);

    switch (key_type) {
        case 0:
            context.n = (void *)rsa_key_buf;

        case 1:
            context.e = (void *)rsa_key_buf;

        case 2:
            context.d = (void *)rsa_key_buf;
            break;

        default:
            TEST_CASE_ASSERT(1 == 0, "choose key type error");
            break;
    }

    switch (rsa_key_len) {
        case 0:
            rsa_key_len_bit = RSA_KEY_BITS_192;
            rsa_test_param.data_len = 24;
            break;

        case 1:
            rsa_key_len_bit = RSA_KEY_BITS_256;
            rsa_test_param.data_len = 32;
            break;

        case 2:
            rsa_key_len_bit = RSA_KEY_BITS_512;
            rsa_test_param.data_len = 64;
            break;

        case 3:
            rsa_key_len_bit = RSA_KEY_BITS_1024;
            rsa_test_param.data_len = 128;
            break;

        case 4:
            rsa_key_len_bit = RSA_KEY_BITS_2048;
            rsa_test_param.data_len = 256;
            break;

        case 5:
            rsa_key_len_bit = RSA_KEY_BITS_3072;
            rsa_test_param.data_len = 384;
            break;

        case 6:
            rsa_key_len_bit = RSA_KEY_BITS_4096;
            rsa_test_param.data_len = 512;
            break;

        default:
            TEST_CASE_ASSERT(1 == 0,"RSA key_len_bit choose error");
            break;
    }

    TEST_PRINT_HEX("rsa_key_buf:", rsa_key_buf, rsa_key_len);
    return 0;
}




int test_rsa_encrypt_sync(char *args)
{
    uint8_t idx;
    uint8_t padding_type;
    uint8_t hash_type;
    int intput_length;
    uint32_t output_length;
    char *p = args;
    csi_error_t ret;
    csi_rsa_t rsa;
    csi_rsa_state_t status;

    memset(rsa_received_data_from_host, 0, RSA_RECV_HOST_DATA_LEN);

    sscanf(args, "%d,%d,%d,%d,%[^,],%d", &idx, &padding_type, &hash_type, rsa_received_data_from_host, &intput_length);
    TEST_CASE_TIPS("key bits: %d", rsa_key_len_bit);
    TEST_CASE_TIPS("padding_type: %d", padding_type);
    TEST_CASE_TIPS("hash_type: %d", hash_type);
    TEST_CASE_TIPS("received source data: %s\n", rsa_received_data_from_host);
    TEST_CASE_TIPS("length_of_source: %d", intput_length);
    get_rsa_test_params(p,&context);
    memset(rsa_source_data, 0, intput_length);
    memset(rsa_target_data, 0x00, intput_length);

    hex_to_array(rsa_received_data_from_host, intput_length, rsa_source_data);
    TEST_PRINT_HEX("SOURCE DATA:", rsa_source_data, intput_length);
    ret = csi_rsa_init(&rsa, idx);
    TEST_CASE_ASSERT_QUIT(ret == CSI_OK, "RSA init error,should return CSI_OK,but returned %d", ret);
    ret = csi_rsa_encrypt(&rsa, &context, (void *)rsa_source_data, intput_length, (void *)rsa_target_data);
    TEST_CASE_ASSERT(ret == CSI_OK, "RSA encrypt error,should return %d, but returned %d ", CSI_OK, ret);

    while (1) {
        csi_rsa_get_state(&rsa, &status);

        if (status.busy == 0) {
            break;
        }
    }

    TEST_PRINT_HEX("ENCRYPTED DATA:", rsa_target_data, intput_length);

    ret = csi_rsa_decrypt(&rsa, &context, rsa_target_data, intput_length, (void *)rsa_temp, &output_length);
    TEST_CASE_ASSERT(ret == CSI_OK, "RSA decypt error,should return %d, but returned %d ", CSI_OK, ret);
    output_length = intput_length;
    TEST_PRINT_HEX("DECRYPTED DATA:", rsa_temp, output_length);

    ret = memcmp(rsa_source_data, rsa_temp, output_length);
    TEST_CASE_ASSERT_QUIT(ret == 0, "The results are not equal\n");
    csi_rsa_uninit(&rsa);
    return 0;
}


int test_rsa_decrypt_sync(char *args)
{
    uint32_t output_length = 32;
    uint8_t idx;
    uint8_t padding_type;
    uint8_t hash_type;
    int intput_length;
    char *p = args;
    csi_error_t ret;
    csi_rsa_t rsa;
    csi_rsa_state_t status;
    
    memset(rsa_source_data, 0, RSA_SOURCE_DATA_LEN);
    memset(rsa_received_data_from_host, 0, RSA_RECV_HOST_DATA_LEN);
    memset(rsa_target_data, 0x00, rsa_test_param.data_len);
    sscanf(args, "%d,%d,%d,%d,%[^,],%d", &idx, &padding_type, &hash_type, rsa_received_data_from_host, &intput_length);
    TEST_CASE_TIPS("key bits: %d", rsa_key_len_bit);
    TEST_CASE_TIPS("padding_type: %d", padding_type);
    TEST_CASE_TIPS("hash_type: %d", hash_type);
    TEST_CASE_TIPS("received source data: %s\n", rsa_received_data_from_host);
    TEST_CASE_TIPS("length_of_source: %d", intput_length);
    get_rsa_test_params(p,&context);

    hex_to_array(rsa_received_data_from_host, intput_length, rsa_source_data);
    TEST_PRINT_HEX("SOURCE DATA:", rsa_source_data, intput_length);

    ret = csi_rsa_init(&rsa, rsa_test_param.idx);
    TEST_CASE_ASSERT_QUIT(ret == CSI_OK, "RSA init error,should return CSI_OK,but returned %d", ret);

    output_length = intput_length;
    ret = csi_rsa_decrypt(&rsa, &context, rsa_source_data, intput_length, (void *)rsa_target_data, &output_length);
    TEST_PRINT_HEX("DECRYPTED DATA:", rsa_target_data, output_length);

    while (1) {
        csi_rsa_get_state(&rsa, &status);

        if (status.busy == 0) {
            break;
        }
    }

    ret = csi_rsa_encrypt(&rsa, &context, (void *)rsa_target_data, intput_length, (void *)rsa_temp);
    TEST_CASE_ASSERT(ret == CSI_OK, "RSA encrypt error,should return %d, but returned %d ", CSI_OK, ret);

    ret = memcmp(rsa_source_data, rsa_temp, intput_length);
    TEST_CASE_ASSERT_QUIT(ret == 0, "The results are not equal");

    csi_rsa_uninit(&rsa);
    return 0;
}


int test_rsa_encrypt_async(char *args)
{
    uint32_t output_length = 32;
    uint8_t idx;
    uint8_t padding_type;
    uint8_t hash_type;
    int intput_length;
    char *p = args;
    csi_error_t ret;
    csi_rsa_t rsa;

    memset(rsa_source_data, 0, RSA_SOURCE_DATA_LEN);
    memset(rsa_received_data_from_host, 0, RSA_RECV_HOST_DATA_LEN);
    memset(rsa_target_data, 0x00, rsa_test_param.data_len);
    sscanf(args, "%d,%d,%d,%d,%[^,],%d", &idx, &padding_type, &hash_type, rsa_received_data_from_host, &intput_length);
    TEST_CASE_TIPS("key bits: %d", rsa_key_len_bit);
    TEST_CASE_TIPS("padding_type: %d", padding_type);
    TEST_CASE_TIPS("hash_type: %d", hash_type);
    TEST_CASE_TIPS("received source data: %s\n", rsa_received_data_from_host);
    TEST_CASE_TIPS("length_of_source: %d", intput_length);
    get_rsa_test_params(p,&context);
    hex_to_array(rsa_received_data_from_host, intput_length, rsa_source_data);
    TEST_PRINT_HEX("SOURCE DATA:", rsa_source_data, intput_length);

    ret = csi_rsa_init(&rsa, rsa_test_param.idx);
    TEST_CASE_ASSERT_QUIT(ret == CSI_OK, "RSA init error,should return CSI_OK,but returned %d", ret);

    csi_rsa_attach_callback(&rsa, rsa_test_cb, NULL);
    rsa_event_finish = 1;
    ret = csi_rsa_encrypt_async(&rsa, &context, (void *)rsa_source_data, intput_length, (void *)rsa_target_data);
    TEST_CASE_ASSERT(ret == CSI_OK, "RSA encrypt error,should return %d, but returned %d ", CSI_OK, ret);

    while (rsa_event_finish) ;

    TEST_PRINT_HEX("ENCRYPTED DATA:", rsa_target_data, rsa_test_param.data_len);

    rsa_event_finish = 1;
    ret = csi_rsa_decrypt_async(&rsa, &context, rsa_target_data, rsa_test_param.data_len, (void *)rsa_temp, &output_length);
    TEST_CASE_ASSERT(ret == CSI_OK, "RSA decrypt error,should return %d, but returned %d ", CSI_OK, ret);

    while (rsa_event_finish) ;

    output_length = intput_length;
    TEST_PRINT_HEX("DECRYPTED DATA:", rsa_temp, output_length);

    ret = memcmp(rsa_source_data, rsa_temp, output_length);
    TEST_CASE_ASSERT_QUIT(ret == 0, "The results are not equal");

    csi_rsa_detach_callback(&rsa);
    csi_rsa_uninit(&rsa);
    return 0;
}


int test_rsa_decrypt_async(char *args)
{
    uint32_t output_length = 32;
    uint8_t idx;
    uint8_t padding_type;
    uint8_t hash_type;
    int intput_length;
    char *p = args;
    csi_error_t ret;
    csi_rsa_t rsa;

    memset(rsa_source_data, 0, RSA_SOURCE_DATA_LEN);
    memset(rsa_received_data_from_host, 0, RSA_RECV_HOST_DATA_LEN);
    memset(rsa_target_data, 0x00, rsa_test_param.data_len);
    sscanf(args, "%d,%d,%d,%d,%[^,],%d", &idx, &padding_type, &hash_type, rsa_received_data_from_host, &intput_length);
    TEST_CASE_TIPS("key bits: %d", rsa_key_len_bit);
    TEST_CASE_TIPS("padding_type: %d", padding_type);
    TEST_CASE_TIPS("hash_type: %d", hash_type);
    TEST_CASE_TIPS("received source data: %s\n", rsa_received_data_from_host);
    TEST_CASE_TIPS("length_of_source: %d", intput_length);
    get_rsa_test_params(p,&context);
    hex_to_array(rsa_received_data_from_host, intput_length, rsa_source_data);
    TEST_PRINT_HEX("SOURCE DATA:", rsa_source_data, intput_length);

    ret = csi_rsa_init(&rsa, rsa_test_param.idx);
    TEST_CASE_ASSERT_QUIT(ret == CSI_OK, "RSA init error,should return CSI_OK,but returned %d", ret);
    csi_rsa_attach_callback(&rsa, rsa_test_cb, NULL);

    output_length = intput_length;
    rsa_event_finish = 1;
    ret = csi_rsa_decrypt_async(&rsa, &context, rsa_source_data, intput_length, (void *)rsa_target_data, &output_length);

    while (rsa_event_finish) ;

    TEST_PRINT_HEX("DECRYPTED DATA:", rsa_target_data, output_length);

    rsa_event_finish = 1;
    ret = csi_rsa_encrypt_async(&rsa, &context, (void *)rsa_target_data, intput_length, (void *)rsa_temp);
    TEST_CASE_ASSERT(ret == CSI_OK, "RSA encrypt error,should return %d, but returned %d ", CSI_OK, ret);

    while (rsa_event_finish) ;

    ret = memcmp(rsa_source_data, rsa_temp, output_length);
    TEST_CASE_ASSERT_QUIT(ret == 0, "The results are not equal");
    csi_rsa_detach_callback(&rsa);
    csi_rsa_uninit(&rsa);
    return 0;
}


int test_rsa_encrypt_decrypt_sync_performance(char *args)
{
    int length_of_source;
    uint32_t output_length = 32;
    uint8_t rand_data = 0;
    int encrypt_times = 0;
    int decrypt_times = 0;
    float performance = 0.0;
    uint32_t elapsed_ms = 0;
    int dummy;
    char *p = args;
    csi_error_t ret;
    csi_rsa_t rsa;
    get_rsa_test_params(p, &context);
    memset(rsa_source_data, 0, RSA_SOURCE_DATA_LEN);
    memset(rsa_received_data_from_host, 0, RSA_RECV_HOST_DATA_LEN);
    memset(rsa_target_data, 0x00, rsa_test_param.data_len);
    sscanf(args, "%d,%d,%d,%d,%d,%d,%d", &dummy, &dummy, &dummy, &dummy, &rand_data, &encrypt_times,&decrypt_times,&length_of_source);

    TEST_CASE_TIPS("key bits: %d", context.key_bits);
    TEST_CASE_TIPS("padding_type: %d", context.padding_type);
    TEST_CASE_TIPS("hash_type: %d", rsa_test_param.hash_type);
    TEST_CASE_TIPS("length_of_source: %d", length_of_source);
    TEST_CASE_TIPS("rsa_test_param.data_len: %d", rsa_test_param.data_len);

    ret = csi_rsa_init(&rsa, rsa_test_param.idx);
    TEST_CASE_ASSERT_QUIT(ret == CSI_OK, "RSA init error, should return CSI_OK,but returned %d", ret);

    switch (rand_data) {
        case 0:
            generate_rand_array2(rsa_source_data, 255, length_of_source);
            tst_timer_restart();

            for (int i = 0; i < encrypt_times; i++) {
                csi_rsa_encrypt(&rsa, &context, (void *)rsa_source_data, length_of_source, (void *)rsa_target_data);
            }

            elapsed_ms =  tst_timer_get_interval() ;
            performance = 1.0 * encrypt_times / elapsed_ms;
            TEST_CASE_TIPS("Keep data constant:test RSA sync encrypt performance: %f times/ms", performance);

            generate_rand_array2(rsa_enc_data, 255, length_of_source);
            tst_timer_restart();

            for (int i = 0; i < decrypt_times; i++) {
                csi_rsa_decrypt(&rsa, &context, rsa_enc_data, rsa_test_param.data_len, (void *)rsa_target_data, &output_length);
            }

            elapsed_ms =  tst_timer_get_interval() ;
            performance =  1.0 * decrypt_times / elapsed_ms;
            TEST_CASE_TIPS("Keep data constant:RSA sync decrypt performance(csi_rsa_decrypt): %f times/ms", performance);
            break;

        case 1:
            tst_timer_restart();

            for (int i = 0; i < encrypt_times; i++) {
                generate_rand_array2(rsa_source_data, 255, length_of_source);
                csi_rsa_encrypt(&rsa, &context, (void *)rsa_source_data, length_of_source, (void *)rsa_target_data);
            }

            elapsed_ms =  tst_timer_get_interval() ;
            performance = 1.0 * encrypt_times / elapsed_ms;
            TEST_CASE_TIPS("Change data every loop:test RSA sync encrypt performance: %f times/ms", performance);

            tst_timer_restart();

            for (int i = 0; i < decrypt_times; i++) {
                generate_rand_array2(rsa_enc_data, 255, length_of_source);
                csi_rsa_decrypt(&rsa, &context, rsa_enc_data, rsa_test_param.data_len, (void *)rsa_target_data, &output_length);
            }

            elapsed_ms =  tst_timer_get_interval() ;
            performance =  1.0 * decrypt_times / elapsed_ms;
            TEST_CASE_TIPS("Change data every loop:test RSA sync decrypt performance: %f times/ms", performance);
            break;

        default:
            TEST_CASE_ASSERT(1 == 0, "Rand data mode choose error");
            break;
    }

    csi_rsa_uninit(&rsa);
    return 0;
}




int test_rsa_encrypt_decrypt_async_performance(char *args)
{
    uint8_t rand_data = 0;
    int length_of_source;
    int encrypt_times = 0;
    int decrypt_times = 0;
    uint32_t output_length = 32;
    int dummy;
    char *p = args;
    csi_error_t ret;
    csi_rsa_t rsa;
    uint32_t elapsed_ms = 0;
    float performance = 0.0;
    get_rsa_test_params(p, &context);
    memset(rsa_source_data, 0, RSA_SOURCE_DATA_LEN);
    memset(rsa_received_data_from_host, 0, RSA_RECV_HOST_DATA_LEN);
    memset(rsa_target_data, 0x00, rsa_test_param.data_len);
    sscanf(args, "%d,%d,%d,%d,%d,%d,%d", &dummy, &dummy, &dummy, &dummy, &rand_data,&encrypt_times,&decrypt_times,&length_of_source);
    TEST_CASE_TIPS("key bits: %d", context.key_bits);
    TEST_CASE_TIPS("padding_type: %d", context.padding_type);
    TEST_CASE_TIPS("hash_type: %d", rsa_test_param.hash_type);
    TEST_CASE_TIPS("length_of_source: %d", length_of_source);
    TEST_CASE_TIPS("rsa_test_param.data_len: %d", rsa_test_param.data_len);

    ret = csi_rsa_init(&rsa, rsa_test_param.idx);
    TEST_CASE_ASSERT_QUIT(ret == CSI_OK, "RSA init error,should return CSI_OK,but returned %d", ret);

    csi_rsa_attach_callback(&rsa, rsa_test_cb, NULL);

    switch (rand_data) {
        case 0:
            generate_rand_array2(rsa_source_data, 255, length_of_source);

            tst_timer_restart();

            for (int i = 0; i < encrypt_times; i++) {
                rsa_event_finish = 1;
                ret = csi_rsa_encrypt_async(&rsa, &context, (void *)rsa_source_data, length_of_source, (void *)rsa_target_data);
                TEST_CASE_ASSERT(ret == CSI_OK, "RSA encrypt error,should return %d, but returned %d ", CSI_OK, ret);

                while (rsa_event_finish) ;
            }

            elapsed_ms =  tst_timer_get_interval() ;
            performance = 1.0 * encrypt_times / elapsed_ms;
            TEST_CASE_TIPS("Keep data constant:test RSA sync encrypt performance: %f times/ms", performance);


            generate_rand_array2(rsa_enc_data, 255, length_of_source);
            tst_timer_restart();

            for (int i = 0; i < decrypt_times; i++) {
                ret = csi_rsa_decrypt_async(&rsa, &context, rsa_target_data, rsa_test_param.data_len, (void *)rsa_source_data, &output_length);
            }

            elapsed_ms =  tst_timer_get_interval() ;
            performance =  1.0 * decrypt_times / elapsed_ms;
            TEST_CASE_TIPS("Keep data constant:RSA async decrypt performance(csi_rsa_decrypt): %f times/ms", performance);
            break;

        case 1:
            tst_timer_restart();

            for (int i = 0; i < encrypt_times; i++) {
                generate_rand_array2(rsa_source_data, 255, length_of_source);
                ret = csi_rsa_encrypt_async(&rsa, &context, (void *)rsa_source_data, length_of_source, (void *)rsa_target_data);
            }

            elapsed_ms =  tst_timer_get_interval() ;
            performance = 1.0 * encrypt_times / elapsed_ms;
            TEST_CASE_TIPS("Change data every loop:test RSA sync encrypt performance: %f times/ms", performance);

            tst_timer_restart();

            for (int i = 0; i < decrypt_times; i++) {
                generate_rand_array2(rsa_enc_data, 255, length_of_source);
                rsa_event_finish = 1;
                ret = csi_rsa_decrypt_async(&rsa, &context, rsa_target_data, rsa_test_param.data_len, (void *)rsa_source_data, &output_length);
                TEST_CASE_ASSERT(ret == CSI_OK, "RSA decrypt error,should return %d, but returned %d ", CSI_OK, ret);

                while (rsa_event_finish) ;
            }

            elapsed_ms =  tst_timer_get_interval() ;
            performance =  1.0 * decrypt_times / elapsed_ms;
            TEST_CASE_TIPS("Change data every loop:test RSA sync decrypt performance: %f times/ms", performance);
            break;

        default:
            break;
    }

    csi_rsa_detach_callback(&rsa);
    csi_rsa_uninit(&rsa);
    return 0;
}


int test_rsa_sign_verify_sync(char *args)
{
    int length_of_source;
    int dummy;
    uint32_t sign_times = 0;
    uint32_t verify_times = 0;
    char *p = args;
    csi_error_t ret;
    csi_rsa_t rsa;
    csi_rsa_state_t status;

    memset(rsa_source_data, 0, RSA_SOURCE_DATA_LEN);
    memset(rsa_received_data_from_host, 0, RSA_RECV_HOST_DATA_LEN);
    memset(rsa_target_data, 0x00, rsa_test_param.data_len);
    sscanf(args, "%d,%d,%d,%d,%[^,],%d,%d,%d", &dummy, &dummy, &dummy, &dummy, rsa_received_data_from_host, &sign_times, &verify_times, &length_of_source);
    TEST_CASE_TIPS("received source data: %s\n", rsa_received_data_from_host);
    TEST_CASE_TIPS("key bits: %d", context.key_bits);
    TEST_CASE_TIPS("padding_type: %d", context.padding_type);
    TEST_CASE_TIPS("hash_type: %d", rsa_test_param.hash_type);
    TEST_CASE_TIPS("length_of_source: %d", length_of_source);
    TEST_CASE_TIPS("rsa_test_param.data_len: %d", rsa_test_param.data_len);

    get_rsa_test_params(p,&context);
    hex_to_array(rsa_received_data_from_host, length_of_source, rsa_source_data);
    TEST_PRINT_HEX("SOURCE DATA:", rsa_source_data, length_of_source);
    ret = csi_rsa_init(&rsa, rsa_test_param.idx);
    TEST_CASE_ASSERT_QUIT(ret == CSI_OK, "RSA init error,should return CSI_OK,but returned %d", ret);

    for (int i = 0; i < sign_times; i++) {
        ret = csi_rsa_sign(&rsa, &context, (void *)rsa_source_data, length_of_source, (void *)rsa_target_data, rsa_test_param.hash_type);
        TEST_CASE_ASSERT(ret == CSI_OK, "RSA sign error,should return %d, but returned %d ", CSI_OK, ret);
        TEST_PRINT_HEX("SIGNed DATA:", rsa_target_data, rsa_test_param.data_len);
    }

    while (1) {
        csi_rsa_get_state(&rsa, &status);

        if (status.busy == 0) {
            break;
        }
    }

    for (int i = 0; i < verify_times; i++) {
        ret = csi_rsa_verify(&rsa, &context, (void *)rsa_source_data, length_of_source, (void *)rsa_target_data, rsa_test_param.data_len, rsa_test_param.hash_type);
        TEST_CASE_ASSERT(ret == true, "RSA verify error,should return CSI_OK,but returned %d", ret);
        TEST_PRINT_HEX("SIGNATURE DATA:", rsa_target_data, rsa_test_param.data_len);
    }

    csi_rsa_uninit(&rsa);
    return 0;
}


int test_rsa_sign_verify_async(char *args)
{
    int length_of_source;
    uint32_t sign_times = 0;
    uint32_t verify_times = 0;
    int dummy;
    char *p = args;
    csi_error_t ret;
    csi_rsa_t rsa;
    csi_rsa_state_t status;
    
    memset(rsa_source_data, 0, RSA_SOURCE_DATA_LEN);
    memset(rsa_received_data_from_host, 0, RSA_RECV_HOST_DATA_LEN);
    memset(rsa_target_data, 0x00, rsa_test_param.data_len);
    sscanf(args, "%d,%d,%d,%d,%[^,],%d,%d,%d", &dummy, &dummy, &dummy, &dummy, rsa_received_data_from_host, &sign_times, &verify_times, &length_of_source);
    TEST_CASE_TIPS("received source data: %s\n", rsa_received_data_from_host);
    TEST_CASE_TIPS("key bits: %d", context.key_bits);
    TEST_CASE_TIPS("padding_type: %d", context.padding_type);
    TEST_CASE_TIPS("hash_type: %d", rsa_test_param.hash_type);
    TEST_CASE_TIPS("length_of_source: %d", length_of_source);
    TEST_CASE_TIPS("rsa_test_param.data_len: %d", rsa_test_param.data_len);

    get_rsa_test_params(p,&context);

    hex_to_array(rsa_received_data_from_host, length_of_source, rsa_source_data);
    TEST_PRINT_HEX("SOURCE DATA:", rsa_source_data, length_of_source);
    ret = csi_rsa_init(&rsa, rsa_test_param.idx);
    TEST_CASE_ASSERT_QUIT(ret == CSI_OK, "RSA init error,should return CSI_OK,but returned %d", ret);
    csi_rsa_attach_callback(&rsa, rsa_test_cb, NULL);

    for (int i = 0; i < sign_times; i++) {
        ret = csi_rsa_sign_async(&rsa, &context, (void *)rsa_source_data, length_of_source, (void *)rsa_target_data, rsa_test_param.hash_type);
        TEST_CASE_ASSERT(ret == CSI_OK, "RSA sign error,should return %d, but returned %d ", CSI_OK, ret);
        TEST_PRINT_HEX("SIGNED DATA:", rsa_target_data, rsa_test_param.data_len);
    }

    while (1) {
        csi_rsa_get_state(&rsa, &status);

        if (status.busy == 0) {
            break;
        }
    }

    for (int i = 0; i < verify_times; i++) {
        ret = csi_rsa_verify_async(&rsa, &context, (void *)rsa_source_data, length_of_source, (void *)rsa_target_data, rsa_test_param.data_len, rsa_test_param.hash_type);
        TEST_CASE_ASSERT(ret == true, "RSA verify error,should return CSI_OK,but returned %d", ret);
        TEST_PRINT_HEX("SIGNATURE DATA:", rsa_target_data, rsa_test_param.data_len);
    }

    while (1) {
        csi_rsa_get_state(&rsa, &status);

        if (status.busy == 0) {
            break;
        }
    }

    csi_rsa_detach_callback(&rsa);
    csi_rsa_uninit(&rsa);
    return 0;
}


int test_rsa_sign_verify_sync_performance(char *args)
{
    int length_of_source;
    uint32_t sign_times = 0;
    uint32_t verify_times = 0;
    uint8_t rand_data = 0;
    int test_loops = 0;
    int dummy;
    csi_error_t ret;
    bool result;
    char *p = args;
    csi_rsa_t rsa;
    csi_rsa_state_t status;
    uint32_t elapsed_ms = 0;
    float performance;
    get_rsa_test_params(p, &context);
    memset(rsa_source_data, 0, RSA_SOURCE_DATA_LEN);
    memset(rsa_target_data, 0x00, rsa_test_param.data_len);
    sscanf(args, "%d,%d,%d,%d,%d,%d,%d,%d,%d", &dummy, &dummy, &dummy, &dummy, &rand_data, &sign_times, &verify_times, &length_of_source, &test_loops);
    TEST_CASE_TIPS("key bits: %d", context.key_bits);
    TEST_CASE_TIPS("padding_type: %d", context.padding_type);
    TEST_CASE_TIPS("hash_type: %d", rsa_test_param.hash_type);
    TEST_CASE_TIPS("length_of_source: %d", length_of_source);
    TEST_CASE_TIPS("rsa_test_param.data_len: %d", rsa_test_param.data_len);

    ret = csi_rsa_init(&rsa, rsa_test_param.idx);
    TEST_CASE_ASSERT_QUIT(ret == CSI_OK, "RSA init error,should return CSI_OK,but returned %d", ret);

    switch (rand_data) {
        case 0:
            generate_rand_array2(rsa_source_data, 255, length_of_source);
            for (int i = 0; i < test_loops; i++) {
                tst_timer_restart();

                for (int i = 0; i < sign_times; i++) {
                    ret = csi_rsa_sign(&rsa, &context, (void *)rsa_source_data, length_of_source, (void *)rsa_target_data, rsa_test_param.hash_type);
                    TEST_CASE_ASSERT(ret == CSI_OK, "RSA sign error,should return %d, but returned %d ", CSI_OK, ret);
                }

                elapsed_ms = tst_timer_get_interval();
                performance = 1.0 * sign_times / elapsed_ms;
                TEST_CASE_TIPS("RSA sync sign performance(csi_rsa_sign): %f times/ms", performance);

                while (1) {
                    csi_rsa_get_state(&rsa, &status);

                    if (status.busy == 0) {
                        break;
                    }
                }

                tst_timer_restart();

                for (int i = 0; i < verify_times; i++) {
                    result = csi_rsa_verify(&rsa, &context, (void *)rsa_source_data, length_of_source, (void *)rsa_target_data, rsa_test_param.data_len, rsa_test_param.hash_type);
                    TEST_CASE_ASSERT_QUIT(result == true, "RSA verify error,should return CSI_OK,but returned %d", ret);
                }

                elapsed_ms = tst_timer_get_interval();
                performance = 1.0 * verify_times / elapsed_ms;
                TEST_CASE_TIPS("Keep data constant:RSA sync verify performance: %f times/ms", performance);
            }
            break;

        case 1:
            for (int i = 0; i < test_loops; i++) {
                generate_rand_array2(rsa_source_data, 255, length_of_source);
                tst_timer_restart();
                for (int i = 0; i < sign_times; i++) {
                    memset(rsa_source_data, 0x00, length_of_source);
                    ret = csi_rsa_sign(&rsa, &context, (void *)rsa_source_data, length_of_source, (void *)rsa_target_data, rsa_test_param.hash_type);
                    TEST_CASE_ASSERT(ret == CSI_OK, "RSA sign error,should return %d, but returned %d ", CSI_OK, ret);
                }

                elapsed_ms = tst_timer_get_interval();
                performance = 1.0 * sign_times / elapsed_ms;
                TEST_CASE_TIPS("Change data every loop:RSA sync sign performance: %f times/ms", performance);

                while (1) {
                    csi_rsa_get_state(&rsa, &status);

                    if (status.busy == 0) {
                        break;
                    }
                }

                tst_timer_restart();
                for (int i = 0; i < verify_times; i++) {
                    result = csi_rsa_verify(&rsa, &context, (void *)rsa_source_data, length_of_source, (void *)rsa_target_data, rsa_test_param.data_len, rsa_test_param.hash_type);
                    TEST_CASE_ASSERT_QUIT(result == true, "RSA verify error,should return CSI_OK,but returned %d", ret);
                }

                elapsed_ms = tst_timer_get_interval();
                performance = 1.0 * verify_times / elapsed_ms;
                TEST_CASE_TIPS("Change data every loop:RSA sync verify performance: %f times/ms", performance);
            }
            break;
        default:
            break;
    }

    csi_rsa_uninit(&rsa);
    return 0;
}


int test_rsa_sign_verify_async_performance(char *args)
{
    int length_of_source;
    int test_loops;
    uint32_t sign_times = 0;
    uint32_t verify_times = 0;
    uint8_t rand_data = 0;
    int dummy;
    csi_error_t ret;
    csi_rsa_t rsa;
    uint32_t elapsed_ms = 0;
    float performance;
    char *p = args;
    get_rsa_test_params(p, &context);
    memset(rsa_source_data, 0, RSA_SOURCE_DATA_LEN);
    memset(rsa_target_data, 0x00, rsa_test_param.data_len);
    sscanf(args, "%d,%d,%d,%d,%d,%d,%d,%d,%d", &dummy, &dummy, &dummy, &dummy, &rand_data, &sign_times, &verify_times, &length_of_source, &test_loops);
    TEST_CASE_TIPS("key bits: %d", context.key_bits);
    TEST_CASE_TIPS("padding_type: %d", context.padding_type);
    TEST_CASE_TIPS("hash_type: %d", rsa_test_param.hash_type);
    TEST_CASE_TIPS("length_of_source: %d", length_of_source);
    TEST_CASE_TIPS("rsa_test_param.data_len: %d", rsa_test_param.data_len);

    ret = csi_rsa_init(&rsa, rsa_test_param.idx);
    TEST_CASE_ASSERT_QUIT(ret == CSI_OK, "RSA init error,should return CSI_OK,but returned %d", ret);
    csi_rsa_attach_callback(&rsa, rsa_test_cb, NULL);

    switch (rand_data) {
        case 0:
            generate_rand_array2(rsa_source_data, 255, length_of_source);

            for (int i = 0; i < test_loops; i++) {
                rsa_event_finish = 1;
                tst_timer_restart();

                for (int i = 0; i < sign_times; i++) {
                    ret = csi_rsa_sign_async(&rsa, &context, (void *)rsa_source_data, length_of_source, (void *)rsa_target_data, rsa_test_param.hash_type);

                    while (rsa_event_finish);
                }

                elapsed_ms = tst_timer_get_interval();
                TEST_CASE_ASSERT(ret == CSI_OK, "RSA sign error,should return %d, but returned %d ", CSI_OK, ret);
                performance = 1.0 * sign_times / elapsed_ms;
                TEST_CASE_TIPS("Keep data constant:RSA async sign performance: %f times/ms", performance);

                rsa_event_finish = 1;
                async_verify_flag = 1;
                tst_timer_restart();

                for (int i = 0; i < verify_times; i++) {
                    ret = csi_rsa_verify_async(&rsa, &context, (void *)rsa_source_data, length_of_source, (void *)rsa_target_data, rsa_test_param.data_len, rsa_test_param.hash_type);
                    TEST_CASE_ASSERT(ret == CSI_OK, "Keep data constant:RSA verify error,should return CSI_OK,but returned %d", ret);

                    while (async_verify_flag);
                }

                elapsed_ms = tst_timer_get_interval();
                performance = 1.0 * verify_times / elapsed_ms;
                TEST_CASE_TIPS("Keep data constant:RSA async verify performance: %f times/ms", performance);
            }
            break;

        case 1:
            for (int i = 0; i < test_loops; i++) {
                rsa_event_finish = 1;
                tst_timer_restart();
                generate_rand_array2(rsa_source_data, 255, length_of_source);

                for (int i = 0; i < sign_times; i++) {
                    ret = csi_rsa_sign_async(&rsa, &context, (void *)rsa_source_data, length_of_source, (void *)rsa_target_data, rsa_test_param.hash_type);

                    while (rsa_event_finish);
                }

                elapsed_ms = tst_timer_get_interval();
                TEST_CASE_ASSERT(ret == CSI_OK, "RSA sign error,should return %d, but returned %d ", CSI_OK, ret);
                performance = 1.0 * sign_times / elapsed_ms;
                TEST_CASE_TIPS("Change data every loop:RSA async sign performance: %f times/ms", performance);

                rsa_event_finish = 1;
                async_verify_flag = 1;
                tst_timer_restart();

                for (int i = 0; i < verify_times; i++) {
                    ret = csi_rsa_verify_async(&rsa, &context, (void *)rsa_source_data, length_of_source, (void *)rsa_target_data, rsa_test_param.data_len, rsa_test_param.hash_type);
                    TEST_CASE_ASSERT(ret == CSI_OK, "RSA verify error.");

                    while (async_verify_flag);
                }

                elapsed_ms = tst_timer_get_interval();
                performance = 1.0 * verify_times / elapsed_ms;
                TEST_CASE_TIPS("Change data every loop:RSA async verify performance: %f times/ms", performance);
            }
            break;

        default:
            TEST_CASE_ASSERT(1 == 0, "Rand data mode choose error");
            break;
    }

    csi_rsa_detach_callback(&rsa);
    csi_rsa_uninit(&rsa);
    return 0;
}



int rsa_encrypt_decrypt_sync_stability(char *args)
{
    int length_of_source;
    uint32_t output_length = 32;
    int test_loops = 0;
    int dummy;
    char *p = args;
    uint32_t encrypt_times = 0;
    uint32_t decrypt_times = 0;
    uint8_t rand_data = 0;
    csi_error_t ret;
    csi_rsa_t rsa;

    memset(rsa_source_data, 0, RSA_SOURCE_DATA_LEN);
    memset(rsa_received_data_from_host, 0, RSA_RECV_HOST_DATA_LEN);
    memset(rsa_target_data, 0x00, rsa_test_param.data_len);
    sscanf(args, "%d,%d,%d,%d,%d,%d,%d,%d,%d", &dummy, &dummy, &dummy, &dummy, &rand_data, &encrypt_times, &decrypt_times, &length_of_source, &test_loops);
    TEST_CASE_TIPS("key bits: %d", context.key_bits);
    TEST_CASE_TIPS("padding_type: %d", context.padding_type);
    TEST_CASE_TIPS("hash_type: %d", rsa_test_param.hash_type);
    TEST_CASE_TIPS("length_of_source: %d", length_of_source);
    TEST_CASE_TIPS("rsa_test_param.data_len: %d", rsa_test_param.data_len);

    get_rsa_test_params(p,&context);

    ret = csi_rsa_init(&rsa, rsa_test_param.idx);
    TEST_CASE_ASSERT_QUIT(ret == CSI_OK, "RSA init error,should return CSI_OK,but returned %d", ret);

    switch (rand_data) {
        case 0:
            generate_rand_array2(rsa_source_data, 255, length_of_source);
            TEST_PRINT_HEX("SOURCE DATA:", rsa_source_data, length_of_source);

            for (int i = 0; i < test_loops; i++) {
                for (int i = 0; i < encrypt_times; i++) {
                    ret = csi_rsa_encrypt(&rsa, &context, (void *)rsa_source_data, length_of_source, (void *)rsa_target_data);
                    TEST_CASE_ASSERT(ret == CSI_OK, "csi_rsa_encrypt_sync should return %d, but returned %d ", CSI_OK, ret);
                    TEST_PRINT_HEX("ENCRYPTED DATA:", rsa_target_data, rsa_test_param.data_len);
                }

                for (int i = 0; i < decrypt_times; i++) {
                    ret = csi_rsa_decrypt(&rsa, &context, rsa_target_data, rsa_test_param.data_len, rsa_dec_data, &output_length);
                    TEST_CASE_ASSERT(ret == CSI_OK, "csi_rsa_decrypt_sync should return %d, but returned %d ", CSI_OK, ret);
                    TEST_PRINT_HEX("DECRYPTED DATA:", rsa_dec_data, output_length);
                }

                ret = memcmp(rsa_source_data, rsa_dec_data, output_length);
                TEST_CASE_ASSERT_QUIT(ret == 0, "Keep data constant:test error,the result is not equal to src data");

            }

            break;

        case 1:
            for (int i = 0; i < test_loops; i++) {
                generate_rand_array2(rsa_source_data, 255, length_of_source);
                TEST_PRINT_HEX("SOURCE DATA:", rsa_source_data, length_of_source);

                for (int i = 0; i < encrypt_times; i++) {
                    ret = csi_rsa_encrypt(&rsa, &context, (void *)rsa_source_data, length_of_source, (void *)rsa_target_data);
                    TEST_CASE_ASSERT(ret == CSI_OK, "RSA encrypt error,should return %d, but returned %d ", CSI_OK, ret);
                    TEST_PRINT_HEX("ENCRYPTED DATA:", rsa_target_data, rsa_test_param.data_len);
                }

                for (int i = 0; i < decrypt_times; i++) {
                    ret = csi_rsa_decrypt(&rsa, &context, rsa_target_data, rsa_test_param.data_len, rsa_dec_data, &output_length);
                    TEST_CASE_ASSERT(ret == CSI_OK, "RSA decrypt error,should return %d, but returned %d ", CSI_OK, ret);
                    TEST_PRINT_HEX("DECRYPTED DATA:", rsa_dec_data, output_length);
                }

                ret = memcmp(rsa_source_data, rsa_dec_data, output_length);
                TEST_CASE_ASSERT_QUIT(ret == 0, "Change data every loop:test error,the result is not equal to src data");
                memset(rsa_source_data, 0x00, length_of_source);
            }
            break;

        default:
            TEST_CASE_ASSERT(1 == 0, "Rand data mode choose error\n");
            break;
    }

    csi_rsa_uninit(&rsa);
    return 0;
}



int rsa_encrypt_decrypt_async_stability(char *args)
{
    int length_of_source;
    int test_loops = 0;
    uint32_t output_length = 32;
    uint32_t encrypt_times = 0;
    uint32_t decrypt_times = 0;
    uint8_t rand_data = 0;
    int dummy;
    char *p = args;
    csi_error_t ret;
    csi_rsa_t rsa;

    memset(rsa_source_data, 0, RSA_SOURCE_DATA_LEN);
    memset(rsa_received_data_from_host, 0, RSA_RECV_HOST_DATA_LEN);
    memset(rsa_target_data, 0x00, rsa_test_param.data_len);
    sscanf(args, "%d,%d,%d,%d,%d,%d,%d,%d,%d", &dummy, &dummy, &dummy, &dummy, &rand_data, &encrypt_times, &decrypt_times, &length_of_source, &test_loops);
    TEST_CASE_TIPS("key bits: %d", context.key_bits);
    TEST_CASE_TIPS("padding_type: %d", context.padding_type);
    TEST_CASE_TIPS("hash_type: %d", rsa_test_param.hash_type);
    TEST_CASE_TIPS("length_of_source: %d", length_of_source);
    TEST_CASE_TIPS("rsa_test_param.data_len: %d", rsa_test_param.data_len);
    get_rsa_test_params(p,&context);


    ret = csi_rsa_init(&rsa, rsa_test_param.idx);
    TEST_CASE_ASSERT_QUIT(ret == CSI_OK, "RSA init error,should return CSI_OK,but returned %d", ret);
    csi_rsa_attach_callback(&rsa, rsa_test_cb, NULL);

    switch (rand_data) {
        case 0:
            generate_rand_array2(rsa_source_data, 255, length_of_source);
            TEST_PRINT_HEX("SOURCE DATA:", rsa_source_data, length_of_source);

            for (int i = 0; i < test_loops; i++) {
                for (int i = 0; i < encrypt_times; i++) {
                    rsa_event_finish = 1;
                    ret = csi_rsa_encrypt_async(&rsa, &context, (void *)rsa_source_data, length_of_source, (void *)rsa_target_data);
                    TEST_CASE_ASSERT(ret == CSI_OK, "RSA encrypt error,should return %d, but returned %d ", CSI_OK, ret);

                    while (rsa_event_finish);

                    TEST_PRINT_HEX("ENCRYPTED DATA:", rsa_target_data, rsa_test_param.data_len);
                }

                for (int i = 0; i < decrypt_times; i++) {
                    rsa_event_finish = 1;
                    ret = csi_rsa_decrypt_async(&rsa, &context, rsa_target_data, length_of_source, rsa_dec_data, &output_length);

                    while (rsa_event_finish);

                    TEST_CASE_ASSERT(ret == CSI_OK, "RSA decrypt error,should return %d, but returned %d ", CSI_OK, ret);
                    TEST_PRINT_HEX("DECRYPTED DATA:", rsa_dec_data, output_length);
                }

                ret = memcmp(rsa_source_data, rsa_dec_data, output_length);
                TEST_CASE_ASSERT_QUIT(ret == 0, "Keep data constant:test error,the result is not equal to src data");
                memset(rsa_dec_data, 0x00, output_length);
                memset(rsa_target_data, 0x00, output_length);
            }

            break;

        case 1:
            for (int i = 0; i < test_loops; i++) {
                generate_rand_array2(rsa_source_data, 255, length_of_source);
                TEST_PRINT_HEX("SOURCE DATA:", rsa_source_data, length_of_source);

                for (int i = 0; i < encrypt_times; i++) {
                    rsa_event_finish = 1;
                    ret = csi_rsa_encrypt_async(&rsa, &context, (void *)rsa_source_data, length_of_source, (void *)rsa_target_data);
                    TEST_CASE_ASSERT(ret == CSI_OK, "RSA encrypt error,should return %d, but returned %d ", CSI_OK, ret);

                    while (rsa_event_finish);

                    TEST_PRINT_HEX("ENCRYPTED DATA:", rsa_target_data, rsa_test_param.data_len);
                }

                for (int i = 0; i < decrypt_times; i++) {
                    rsa_event_finish = 1;
                    ret = csi_rsa_decrypt_async(&rsa, &context, rsa_target_data, length_of_source, rsa_dec_data, &output_length);

                    while (rsa_event_finish);

                    TEST_CASE_ASSERT(ret == CSI_OK, "RSA decrypt error,should return %d, but returned %d ", CSI_OK, ret);
                    TEST_PRINT_HEX("DECRYPTED DATA:", rsa_dec_data, output_length);
                }

                ret = memcmp(rsa_source_data, rsa_dec_data, output_length);
                TEST_CASE_ASSERT_QUIT(ret == 0, "Change data every loop:test error,the result is not equal to src data");
                memset(rsa_dec_data, 0x00, output_length);
                memset(rsa_target_data, 0x00, output_length);
            }

            break;

        default:
            break;
    }

    csi_rsa_detach_callback(&rsa);
    csi_rsa_uninit(&rsa);
    return 0;
}



int rsa_sign_verify_sync_stability(char *args)
{
    int length_of_source;
    bool result;
    int test_loops = 0;
    int dummy;
    char *p = args;
    csi_error_t ret;
    csi_rsa_t rsa;
    csi_rsa_state_t status;
    uint32_t sign_times = 0;
    uint32_t verify_times = 0;
    uint8_t rand_data = 0;

    memset(rsa_source_data, 0, RSA_SOURCE_DATA_LEN);
    memset(rsa_target_data, 0x00, rsa_test_param.data_len);
    sscanf(args, "%d,%d,%d,%d,%d,%d,%d,%d,%d", &dummy, &dummy, &dummy, &dummy, &rand_data, &sign_times, &verify_times, &length_of_source, &test_loops);
    TEST_CASE_TIPS("key bits: %d", context.key_bits);
    TEST_CASE_TIPS("padding_type: %d", context.padding_type);
    TEST_CASE_TIPS("hash_type: %d", rsa_test_param.hash_type);
    TEST_CASE_TIPS("length_of_source: %d", length_of_source);
    TEST_CASE_TIPS("rsa_test_param.data_len: %d", rsa_test_param.data_len);
    get_rsa_test_params(p,&context);

    ret = csi_rsa_init(&rsa, rsa_test_param.idx);
    TEST_CASE_ASSERT_QUIT(ret == CSI_OK, "RSA init error,should return CSI_OK,but returned %d", ret);

    switch (rand_data) {
        case 0:
            generate_rand_array2(rsa_source_data, 255, length_of_source);
            TEST_PRINT_HEX("SOURCE DATA:", rsa_source_data, length_of_source);

            for (int i = 0; i < test_loops; i++) {
                for (int i = 0; i < sign_times; i++) {
                    ret = csi_rsa_sign(&rsa, &context, (void *)rsa_source_data, length_of_source, (void *)rsa_target_data, rsa_test_param.hash_type);
                    TEST_CASE_ASSERT_QUIT(ret == CSI_OK, "RSA sign error,should return %d, but returned %d ", CSI_OK, ret);
                    TEST_PRINT_HEX("SIGNED DATA:", rsa_target_data, rsa_test_param.data_len);
                }

                while (1) {
                    csi_rsa_get_state(&rsa, &status);

                    if (status.busy == 0) {
                        break;
                    }
                }

                for (int i = 0; i < verify_times; i++) {
                    result = csi_rsa_verify(&rsa, &context, (void *)rsa_source_data, length_of_source, (void *)rsa_target_data, rsa_test_param.data_len, rsa_test_param.hash_type);
                    TEST_CASE_ASSERT_QUIT(result == true, "RSA verify error,should return %d, but returned %d ", CSI_OK, ret);
                }

            }

            memset(rsa_source_data, 0x00, length_of_source);
            break;

        case 1:
            for (int i = 0; i < test_loops; i++) {
                generate_rand_array2(rsa_source_data, 255, length_of_source);
                TEST_PRINT_HEX("SOURCE DATA:", rsa_source_data, length_of_source);

                for (int i = 0; i < sign_times; i++) {
                    ret = csi_rsa_sign(&rsa, &context, (void *)rsa_source_data, length_of_source, (void *)rsa_target_data, rsa_test_param.hash_type);
                    TEST_CASE_ASSERT_QUIT(ret == CSI_OK, "RSA sign error,should return %d, but returned %d ", CSI_OK, ret);
                    TEST_PRINT_HEX("SIGNED DATA:", rsa_target_data, rsa_test_param.data_len);
                }

                while (1) {
                    csi_rsa_get_state(&rsa, &status);

                    if (status.busy == 0) {
                        break;
                    }
                }

                for (int i = 0; i < verify_times; i++) {
                    result = csi_rsa_verify(&rsa, &context, (void *)rsa_source_data, length_of_source, (void *)rsa_target_data, rsa_test_param.data_len, rsa_test_param.hash_type);
                    TEST_CASE_ASSERT_QUIT(result == true, "RSA verify error,should return %d, but returned %d ", CSI_OK, ret);
                }

                memset(rsa_source_data, 0x00, length_of_source);
            }

            break;

        default:
            TEST_CASE_ASSERT(1 == 0, "Rand data mode error");
            break;
    }

    csi_rsa_uninit(&rsa);
    return 0;
}



int rsa_sign_verify_async_stability(char *args)
{
    int length_of_source;
    bool result;
    int test_loops = 0;
    int dummy;
    char *p = args;
    csi_error_t ret;
    csi_rsa_t rsa;
    csi_rsa_state_t status;
    uint32_t sign_times = 0;
    uint32_t verify_times = 0;
    uint8_t rand_data = 0;

    memset(rsa_source_data, 0, RSA_SOURCE_DATA_LEN);
    memset(rsa_target_data, 0x00, rsa_test_param.data_len);
    sscanf(args, "%d,%d,%d,%d,%d,%d,%d,%d,%d", &dummy, &dummy, &dummy, &dummy, &rand_data, &sign_times, &verify_times, &length_of_source, &test_loops);
    TEST_CASE_TIPS("key bits: %d", context.key_bits);
    TEST_CASE_TIPS("padding_type: %d", context.padding_type);
    TEST_CASE_TIPS("hash_type: %d", rsa_test_param.hash_type);
    TEST_CASE_TIPS("length_of_source: %d", length_of_source);
    TEST_CASE_TIPS("rsa_test_param.data_len: %d", rsa_test_param.data_len);

    get_rsa_test_params(p,&context);

    ret = csi_rsa_init(&rsa, rsa_test_param.idx);
    TEST_CASE_ASSERT_QUIT(ret == CSI_OK, "RSA init error,should return CSI_OK,but returned %d", ret);
    csi_rsa_attach_callback(&rsa, rsa_test_cb, NULL);

    switch (rand_data) {
        case 0:
            generate_rand_array2(rsa_source_data, 255, length_of_source);
            TEST_PRINT_HEX("SOURCE DATA:", rsa_source_data, length_of_source);

            for (int i = 0; i < test_loops; i++) {
                for (int i = 0; i < sign_times; i++) {
                    ret = csi_rsa_sign(&rsa, &context, (void *)rsa_source_data, length_of_source, (void *)rsa_target_data, rsa_test_param.hash_type);
                    TEST_CASE_ASSERT_QUIT(ret == CSI_OK, "RSA sign error,should return %d, but returned %d ", CSI_OK, ret);
                    TEST_PRINT_HEX("SIGNED DATA:", rsa_target_data, rsa_test_param.data_len);
                }

                while (1) {
                    csi_rsa_get_state(&rsa, &status);

                    if (status.busy == 0) {
                        break;
                    }
                }

                for (int i = 0; i < verify_times; i++) {
                    result = csi_rsa_verify(&rsa, &context, (void *)rsa_source_data, length_of_source, (void *)rsa_target_data, rsa_test_param.data_len, rsa_test_param.hash_type);
                    TEST_CASE_ASSERT_QUIT(result == true, "RSA verify error,should return %d, but returned %d ", CSI_OK, ret);
                }

            }

            memset(rsa_source_data, 0x00, length_of_source);
            break;

        case 1:
            for (int i = 0; i < test_loops; i++) {
                generate_rand_array2(rsa_source_data, 255, length_of_source);
                TEST_PRINT_HEX("SOURCE DATA:", rsa_source_data, length_of_source);

                for (int i = 0; i < sign_times; i++) {
                    rsa_event_finish = 1;
                    ret = csi_rsa_sign(&rsa, &context, (void *)rsa_source_data, length_of_source, (void *)rsa_target_data, rsa_test_param.hash_type);
                    TEST_CASE_ASSERT_QUIT(ret == CSI_OK, "RSA sign error,should return %d, but returned %d ", CSI_OK, ret);

                    while (rsa_event_finish);

                    TEST_PRINT_HEX("SIGNED DATA:", rsa_target_data, rsa_test_param.data_len);
                }

                while (1) {
                    csi_rsa_get_state(&rsa, &status);

                    if (status.busy == 0) {
                        break;
                    }
                }

                for (int i = 0; i < verify_times; i++) {
                    async_verify_flag = 1;
                    ret = csi_rsa_verify_async(&rsa, &context, (void *)rsa_source_data, length_of_source, (void *)rsa_target_data, rsa_test_param.data_len, rsa_test_param.hash_type);

                    while (async_verify_flag);

                    TEST_CASE_ASSERT(ret == CSI_OK, "RSA verify error,should return CSI_OK,but returned %d", ret);
                    TEST_PRINT_HEX("SIGNATURE DATA:", rsa_target_data, rsa_test_param.data_len);
                }

                memset(rsa_source_data, 0x00, length_of_source);
            }

            break;

        default:
            TEST_CASE_ASSERT(1 == 0, "Rand data mode error");
            break;
    }

    csi_rsa_detach_callback(&rsa);
    csi_rsa_uninit(&rsa);
    return 0;
}
