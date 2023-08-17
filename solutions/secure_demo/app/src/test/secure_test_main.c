
#include <devices/spi.h>
#include <se_device.h>
#include <se_aes.h>
#include <se_ecdh.h>
#include <se_ecdsa.h>
#include <se_rsa.h>
#include <se_sha.h>
#include "yunit.h"
#include "se_keystore.h"

extern void sc_test_ecc(void);
extern void sc_test_aes(void);

extern void se_test_rsa(void);
extern void se_test_aes(void);
extern void se_test_device(void);
extern void se_test_sstorage(void);
extern void se_test_hmac(void);
extern void se_test_rng(void);
extern void se_test_sha(void);
extern void yunit_test_main(void);

void se_test_all(void)
{
    se_test_rsa();

    se_test_aes();

	se_test_sstorage();

    se_test_hmac();

    se_test_rng();

    se_test_sha();
}

void sc_test_all(void)
{
    sc_test_ecc();
    sc_test_aes();
}

void secure_se_test_main(void)
{
    se_test_all();

    yunit_test_main();

}

void secure_sc_test_main(void)
{
    sc_test_all();

    yunit_test_main();

}
