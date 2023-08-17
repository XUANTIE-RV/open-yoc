#ifndef LINK_VISUAL_MBEDTLS_H
#define LINK_VISUAL_MBEDTLS_H


/*
 * @NOTICE:mbedtls露出是为了避免不同版本mbedtls的编译兼容问题，因此本文件一般不需要修改。
 *
 * */

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

static const int LV_ENUM_MBEDTLS_SSL_IS_CLIENT = 0;

static const int LV_ENUM_MBEDTLS_SSL_TRANSPORT_STREAM = 0;   /*!< TLS      */
static const int LV_ENUM_MBEDTLS_SSL_TRANSPORT_DATAGRAM = 1;   /*!< DTLS     */

static const int LV_ENUM_MBEDTLS_SSL_PRESET_DEFAULT = 0;

static const int LV_ENUM_MBEDTLS_SSL_VERIFY_NONE = 0;
static const int LV_ENUM_MBEDTLS_SSL_VERIFY_OPTIONAL = 1;
static const int LV_ENUM_MBEDTLS_SSL_VERIFY_REQUIRED = 2;

static const int LV_MBEDTLS_ERR_SSL_WANT_READ = -0x6900;  /**< Connection requires a read call. */
static const int LV_MBEDTLS_ERR_SSL_WANT_WRITE = -0x6880;  /**< Connection requires a write call. */


extern void lv_mbedtls_ssl_context_create(void **ssl_context);
extern void lv_mbedtls_ssl_context_destroy(void *ssl_context);

extern void lv_mbedtls_ssl_config_create(void **ssl_config);
extern void lv_mbedtls_ssl_config_destroy(void *ssl_config);

extern void lv_mbedtls_x509_crt_create(void **x509_crt);
extern void lv_mbedtls_x509_crt_destroy(void *x509_crt);

extern void lv_mbedtls_ctr_drbg_context_create(void **ctr_drbg_context);
extern void lv_mbedtls_ctr_drbg_context_destroy(void *ctr_drbg_context);

extern void lv_mbedtls_entropy_context_create(void **entropy_context);
extern void lv_mbedtls_entropy_context_destroy(void *entropy_context);

extern int lv_mbedtls_ctr_drbg_seed(void *ctr_drbg_contex, void *p_entropy, const unsigned char *custom, size_t len);

extern int lv_mbedtls_x509_crt_parse(void *x509_crt_chain, const unsigned char *buf, size_t buflen);

extern int lv_mbedtls_ssl_config_defaults(void *ssl_config, int endpoint, int transport, int preset);

extern void lv_mbedtls_ssl_conf_authmode(void *ssl_config, int authmode);

extern void lv_mbedtls_ssl_conf_ca_chain(void *ssl_config, void *ca_chain, void *ca_crl);

extern void lv_mbedtls_ssl_conf_rng(void *ssl_config, void *p_rng);

extern int lv_mbedtls_ssl_setup(void *ssl_context, void *ssl_config);

extern void lv_mbedtls_ssl_set_bio(void *ssl_context, void *p_bio,
                                int (*f_read)(void *ctx, const unsigned char *buf, size_t len),
                                int (*f_recv)(void *ctx, unsigned char *buf, size_t len));

extern int lv_mbedtls_ssl_handshake(void *ssl_context);

extern int lv_mbedtls_ssl_read(void *ssl_context, unsigned char *buf, size_t len);

extern int lv_mbedtls_ssl_write(void *ssl_context, const unsigned char *buf, size_t len);

#ifdef __cplusplus
}
#endif

#endif //LINK_VISUAL_MBEDTLS_H
