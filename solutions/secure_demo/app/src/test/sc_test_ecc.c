/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#include <aos/kernel.h>
#include <ulog/ulog.h>
#include <sec_crypto_ecdsa.h>
#include <sec_crypto_ecdh.h>
#include <stdio.h>
#include <string.h>
#include "se_device.h"
#include "yunit.h"


int sc_ecdsa_test(void)
{
    static const uint8_t private[] = {
        0xce,0x35,0x97,0xca,0x50,0x73,0x6c,0x68,0x57,0xe6,0xbd,0xe1,0x3f,0x41,0x68,0xbf,
        0xa1,0x0c,0xf1,0x37,0xa6,0x9e,0xfd,0xab,0x2d,0x12,0x39,0x8b,0xe4,0x43,0x69,0x8f,
    };

    static const uint8_t public[] = {
        0x68,0x27,0x99,0xce,0x79,0x4c,0x48,0x5a,0xc0,0x9e,0x6e,0x92,0x0a,0x8b,0xd7,0xc4,
        0x50,0x65,0x8f,0x22,0x2c,0x4f,0x35,0xb4,0xbe,0x98,0x9c,0x7c,0xe8,0xd2,0x25,0x2c,
        0xc7,0xc7,0x41,0xf0,0x91,0x28,0xe7,0x48,0x97,0xc5,0x1f,0x12,0x62,0xc2,0x29,0x99,
        0xba,0x66,0x32,0x83,0x88,0xe0,0x06,0x19,0xf2,0x34,0x62,0xc0,0xa3,0xd9,0xb4,0x30
    };
    uint32_t ret = 0;
    sc_ecdsa_t ecdsa;
    uint8_t hash[32] = {0};
    uint8_t sig[64] = {0};
    uint32_t sig_len = 0;

    ret = sc_ecdsa_init(&ecdsa, 0);
    CHECK_RET_WITH_RET(ret == 0, -1);

    ret = sc_ecdsa_load_curve(&ecdsa, SC_CURVES_SECP521R1);
    CHECK_RET_WITH_RET(ret == 0, -1);

    memset(hash, 0x55, sizeof(hash));

    ret = sc_ecdsa_sign(&ecdsa, private, sizeof(private), hash, sizeof(hash), sig, &sig_len);
    CHECK_RET_WITH_RET(ret == 0, -1);

    ret = sc_ecdsa_verify(&ecdsa, public, sizeof(public), hash, sizeof(hash), sig, sig_len);
    CHECK_RET_WITH_RET(ret == 0, -1);

    sc_ecdsa_uninit(&ecdsa);

    return 0;
}

int sc_ecdh_test(void)
{
    uint32_t ret = 0;
    sc_ecdh_t ecdh;
    uint8_t private_key_1[32] = {0};
    uint8_t public_key_1[64] = {0};
    uint8_t private_key_2[32] = {0};
    uint8_t public_key_2[64] = {0};
    uint8_t sk_1[32] = {0};
    uint32_t sk_len_1 = 0;
    uint8_t sk_2[32] = {0};
    uint32_t sk_len_2 = 0;

    ret = sc_ecdh_init(&ecdh, 0);
    CHECK_RET_WITH_RET(ret == 0, -1);

    ret = sc_ecdh_load_curve(&ecdh, SC_CURVES_SECP521R1);
    CHECK_RET_WITH_RET(ret == 0, -1);

    ret = sc_ecdh_gen_keypair(&ecdh, private_key_1, public_key_1);
    CHECK_RET_WITH_RET(ret == 0, -1);

    ret = sc_ecdh_gen_keypair(&ecdh, private_key_2, public_key_2);
    CHECK_RET_WITH_RET(ret == 0, -1);

    ret = sc_ecdh_calc_secret(&ecdh, private_key_1, public_key_2, sk_1, &sk_len_1);
    CHECK_RET_WITH_RET(ret == 0, -1);

    ret = sc_ecdh_calc_secret(&ecdh, private_key_2, public_key_1, sk_2, &sk_len_2);
    CHECK_RET_WITH_RET(ret == 0, -1);

    CHECK_RET_WITH_RET(sk_len_1 == sk_len_2, -1);
    ret = memcmp(sk_1, sk_2, sk_len_1);
    CHECK_RET_WITH_RET(ret == 0, -1);

    sc_ecdh_uninit(&ecdh);

    return 0;
}

static void sc_test_ecc_demo(void)
{
    int ret = 0;
    ret = sc_ecdsa_test();
    YUNIT_ASSERT_MSG_QA(ret == 0, "the sc_ecdsa_test is %d", ret);

    ret = sc_ecdh_test();
    YUNIT_ASSERT_MSG_QA(ret == 0, "the sc_ecdsa_test is %d", ret);
}

void sc_test_ecc(void)
{
    /***************SC ECC TEST START****************/

    add_yunit_test("sc_ecc", &sc_test_ecc_demo);

    /***************SC ECC TEST END****************/
}