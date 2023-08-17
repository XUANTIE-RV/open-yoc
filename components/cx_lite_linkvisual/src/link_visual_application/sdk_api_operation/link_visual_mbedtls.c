#include "link_visual_mbedtls.h"

#include <stdlib.h>

#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/ssl.h"

/*
 * @NOTICE:mbedtls露出是为了避免不同mbedtls的编译兼容问题，因此本文件一般不需要修改。
 *
 * */

void lv_mbedtls_ssl_context_create(void **ssl_context) {
    if (ssl_context) {
        *ssl_context = (mbedtls_ssl_context *)malloc(sizeof(mbedtls_ssl_context));
        mbedtls_ssl_init((mbedtls_ssl_context *)(*ssl_context));
    }
}

void lv_mbedtls_ssl_context_destroy(void *ssl_context) {
    if (ssl_context) {
        mbedtls_ssl_free((mbedtls_ssl_context *)ssl_context);
        free((mbedtls_ssl_context *)ssl_context);
    }
}

void lv_mbedtls_ssl_config_create(void **ssl_config) {
    if (ssl_config) {
        *ssl_config = (mbedtls_ssl_config *)malloc(sizeof(mbedtls_ssl_config));
        mbedtls_ssl_config_init((mbedtls_ssl_config *)(*ssl_config));
    }
}

void lv_mbedtls_ssl_config_destroy(void *ssl_config) {
    if (ssl_config) {
        mbedtls_ssl_config_free((mbedtls_ssl_config *)ssl_config);
        free((mbedtls_ssl_config *)ssl_config);
    }
}

void lv_mbedtls_x509_crt_create(void **x509_crt) {
    if (x509_crt) {
        *x509_crt = (mbedtls_x509_crt *)malloc(sizeof(mbedtls_x509_crt));
        mbedtls_x509_crt_init((mbedtls_x509_crt *)(*x509_crt));
    }
}

void lv_mbedtls_x509_crt_destroy(void *x509_crt) {
    if (x509_crt) {
        mbedtls_x509_crt_free((mbedtls_x509_crt *)x509_crt);
        free((mbedtls_x509_crt *)x509_crt);
    }
}

void lv_mbedtls_ctr_drbg_context_create(void **ctr_drbg_context) {
    if (ctr_drbg_context) {
        *ctr_drbg_context = (mbedtls_ctr_drbg_context *)malloc(sizeof(mbedtls_ctr_drbg_context));
        mbedtls_ctr_drbg_init((mbedtls_ctr_drbg_context *)(*ctr_drbg_context));
    }
}

void lv_mbedtls_ctr_drbg_context_destroy(void *ctr_drbg_context) {
    if (ctr_drbg_context) {
        mbedtls_ctr_drbg_free((mbedtls_ctr_drbg_context *)ctr_drbg_context);
        free((mbedtls_ctr_drbg_context *)ctr_drbg_context);
    }
}

void lv_mbedtls_entropy_context_create(void **entropy_context) {
    if (entropy_context) {
        *entropy_context = (mbedtls_entropy_context *)malloc(sizeof(mbedtls_entropy_context));
        mbedtls_entropy_init((mbedtls_entropy_context *)(*entropy_context));
    }
}

void lv_mbedtls_entropy_context_destroy(void *entropy_context) {
    if (entropy_context) {
        mbedtls_entropy_free((mbedtls_entropy_context *)entropy_context);
        free((mbedtls_entropy_context *)entropy_context);
    }
}

int lv_mbedtls_ctr_drbg_seed(void *ctr_drbg_context,  void *p_entropy, const unsigned char *custom, size_t len) {
    return mbedtls_ctr_drbg_seed((mbedtls_ctr_drbg_context *)ctr_drbg_context, mbedtls_entropy_func, p_entropy, custom, len);
}

int lv_mbedtls_x509_crt_parse(void *x509_crt_chain, const unsigned char *buf, size_t buflen) {
    return mbedtls_x509_crt_parse(x509_crt_chain, buf, buflen);
}

int lv_mbedtls_ssl_config_defaults(void *ssl_config, int endpoint, int transport, int preset) {
    return mbedtls_ssl_config_defaults((mbedtls_ssl_config *)ssl_config, endpoint, transport, preset);
}

void lv_mbedtls_ssl_conf_authmode(void *ssl_config, int authmode) {
    mbedtls_ssl_conf_authmode((mbedtls_ssl_config *)ssl_config, authmode);
}

void lv_mbedtls_ssl_conf_ca_chain(void *ssl_config, void *ca_chain, void *ca_crl) {
    mbedtls_ssl_conf_ca_chain((mbedtls_ssl_config *)ssl_config, (mbedtls_x509_crt *)ca_chain, (mbedtls_x509_crl *)ca_crl);
}

void lv_mbedtls_ssl_conf_rng(void *ssl_config, void *p_rng) {
    mbedtls_ssl_conf_rng((mbedtls_ssl_config *)ssl_config, mbedtls_ctr_drbg_random, p_rng);
}

int lv_mbedtls_ssl_setup(void *ssl_context, void *ssl_config) {
    return mbedtls_ssl_setup((mbedtls_ssl_context *)ssl_context, (mbedtls_ssl_config *)ssl_config);
}

void lv_mbedtls_ssl_set_bio(void *ssl_context, void *p_bio,
                                   int (*f_send)(void *ctx, const unsigned char *buf, size_t len),
                                   int (*f_recv)(void *ctx, unsigned char *buf, size_t len)) {
    mbedtls_ssl_set_bio((mbedtls_ssl_context *)ssl_context, p_bio, f_send, f_recv, NULL);
}

int lv_mbedtls_ssl_handshake(void *ssl_context) {
    return mbedtls_ssl_handshake((mbedtls_ssl_context *)ssl_context);
}

int lv_mbedtls_ssl_read(void *ssl_context, unsigned char *buf, size_t len) {
    return mbedtls_ssl_read((mbedtls_ssl_context *)ssl_context, buf, len);
}

int lv_mbedtls_ssl_write(void *ssl_context, const unsigned char *buf, size_t len) {
    return mbedtls_ssl_write((mbedtls_ssl_context *)ssl_context, buf, len);
}