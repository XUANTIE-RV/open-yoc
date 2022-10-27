/*
 *  Minimal configuration for TLS 1.2 with PSK and AES-CCM ciphersuites
 *
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of mbed TLS (https://tls.mbed.org)
 */
/*
 * Minimal configuration for TLS 1.2 with PSK and AES-CCM ciphersuites
 * Distinguishing features:
 * - no bignum, no PK, no X509
 * - fully modern and secure (provided the pre-shared keys have high entropy)
 * - very low record overhead with CCM-8
 * - optimized for low RAM usage
 *
 * See README.txt for usage instructions.
 */
#ifndef MBEDTLS_CONFIG_H
#define MBEDTLS_CONFIG_H

#define MBEDTLS_CONFIG_TLS
#define MBEDTLS_CONFIG_X509
#define MBEDTLS_CONFIG_CRYPTO_SHA1
#define MBEDTLS_CONFIG_CRYPTO_SHA256
#define MBEDTLS_CONFIG_CRYPTO_SHA512
#define MBEDTLS_CONFIG_DTLS
#define MBEDTLS_CONFIG_CRYPTO_MODE_CBC
#define MBEDTLS_CONFIG_CRYPTO_MODE_CTR
#define MBEDTLS_CONFIG_CRYPTO_MODE_CFB
#define MBEDTLS_CONFIG_CRYPTO_AES
#define MBEDTLS_CONFIG_CRYPTO_MD5
//#define MBEDTLS_CONFIG_TLS_DEBUG


/* System support */
#define MBEDTLS_HAVE_ASM
#define MBEDTLS_NO_UDBL_DIVISION
#define MBEDTLS_NO_64BIT_MULTIPLICATION
#define MBEDTLS_HAVE_TIME
#define MBEDTLS_PLATFORM_MEMORY

/* mbed TLS feature support */
#define MBEDTLS_CIPHER_PADDING_PKCS7
#define MBEDTLS_CIPHER_PADDING_ONE_AND_ZEROS
#define MBEDTLS_CIPHER_PADDING_ZEROS_AND_LEN
#define MBEDTLS_CIPHER_PADDING_ZEROS
#define MBEDTLS_REMOVE_ARC4_CIPHERSUITES
#define MBEDTLS_ERROR_STRERROR_DUMMY
#define MBEDTLS_GENPRIME
#define MBEDTLS_PKCS1_V15
#define MBEDTLS_PKCS1_V21
#define MBEDTLS_THREADING_ALT
#define MBEDTLS_VERSION_FEATURES
#define MBEDTLS_NET_C_ALT

#if defined(MBEDTLS_CONFIG_CRYPTO_AES_ROM_TABLES)
#define MBEDTLS_AES_ROM_TABLES
#endif
#if defined(MBEDTLS_CONFIG_CRYPTO_AES_FEWER_TABLES)
#define MBEDTLS_AES_FEWER_TABLES
#endif

#if defined(MBEDTLS_CONFIG_CRYPTO_MODE_CBC)
#define MBEDTLS_CIPHER_MODE_CBC
#endif
#if defined(MBEDTLS_CONFIG_CRYPTO_MODE_CFB)
#define MBEDTLS_CIPHER_MODE_CFB
#endif
#if defined(MBEDTLS_CONFIG_CRYPTO_MODE_CTR)
#define MBEDTLS_CIPHER_MODE_CTR
#endif
#if defined(MBEDTLS_CONFIG_CRYPTO_MODE_OFB)
#define MBEDTLS_CIPHER_MODE_OFB
#endif
#if defined(MBEDTLS_CONFIG_CRYPTO_MODE_XTS)
#define MBEDTLS_CIPHER_MODE_XTS
#endif

#if defined(MBEDTLS_CONFIG_SELFTEST)
#define MBEDTLS_SELF_TEST
#endif

#if defined(MBEDTLS_CONFIG_TLS)
#define MBEDTLS_KEY_EXCHANGE_RSA_ENABLED
#define MBEDTLS_SSL_ALL_ALERT_MESSAGES
#define MBEDTLS_SSL_ENCRYPT_THEN_MAC
#define MBEDTLS_SSL_EXTENDED_MASTER_SECRET
#define MBEDTLS_SSL_FALLBACK_SCSV
#define MBEDTLS_SSL_RENEGOTIATION
#define MBEDTLS_SSL_MAX_FRAGMENT_LENGTH
#define MBEDTLS_SSL_PROTO_TLS1_2
#if defined(MBEDTLS_CONFIG_DTLS)
#define MBEDTLS_SSL_PROTO_DTLS
#endif
#define MBEDTLS_SSL_ALPN
#define MBEDTLS_SSL_DTLS_ANTI_REPLAY
#define MBEDTLS_SSL_DTLS_HELLO_VERIFY
#define MBEDTLS_SSL_DTLS_CLIENT_PORT_REUSE
#define MBEDTLS_SSL_DTLS_BADMAC_LIMIT
#define MBEDTLS_SSL_SESSION_TICKETS
#define MBEDTLS_SSL_EXPORT_KEYS
#define MBEDTLS_SSL_SERVER_NAME_INDICATION
#define MBEDTLS_SSL_TRUNCATED_HMAC
#endif /* MBEDTLS_CONFIG_TLS */

#if defined(MBEDTLS_CONFIG_X509)
#define MBEDTLS_X509_CHECK_KEY_USAGE
#define MBEDTLS_X509_CHECK_EXTENDED_KEY_USAGE
#define MBEDTLS_X509_RSASSA_PSS_SUPPORT
#endif

/* mbed TLS modules */
#define MBEDTLS_ASN1_PARSE_C
#define MBEDTLS_ASN1_WRITE_C
#define MBEDTLS_BASE64_C
#define MBEDTLS_BIGNUM_C
#define MBEDTLS_CIPHER_C
#define MBEDTLS_HKDF_C
#define MBEDTLS_HMAC_DRBG_C
#define MBEDTLS_OID_C
#define MBEDTLS_CERTS_C
#define MBEDTLS_PEM_PARSE_C
#define MBEDTLS_MD_C
#define MBEDTLS_PK_C
#define MBEDTLS_PK_PARSE_C
#define MBEDTLS_PKCS5_C
#define MBEDTLS_PKCS12_C
#define MBEDTLS_PLATFORM_C
#define MBEDTLS_THREADING_C
#define MBEDTLS_TIMING_C
#define MBEDTLS_VERSION_C
#define MBEDTLS_RSA_C

#if defined(MBEDTLS_CONFIG_CRYPTO_AES)
#define MBEDTLS_AES_C
#endif
#if defined(MBEDTLS_CONFIG_CRYPTO_ARC4)
#define MBEDTLS_ARC4_C
#endif
#if defined(MBEDTLS_CONFIG_CRYPTO_BLOWFISH)
#define MBEDTLS_BLOWFISH_C
#endif
#if defined(MBEDTLS_CONFIG_CRYPTO_CAMELLIA)
#define MBEDTLS_CAMELLIA_C
#endif
#if defined(MBEDTLS_CONFIG_CRYPTO_ARIA)
#define MBEDTLS_ARIA_C
#endif
#if defined(MBEDTLS_CONFIG_CRYPTO_MODE_CCM)
#define MBEDTLS_CCM_C
#endif
#if defined(MBEDTLS_CONFIG_CRYPTO_MODE_GCM)
#define MBEDTLS_GCM_C
#endif
#if defined(MBEDTLS_CONFIG_CRYPTO_CHACHA20)
#define MBEDTLS_CHACHA20_C
#endif
#if defined(MBEDTLS_CONFIG_CRYPTO_POLY1305)
#define MBEDTLS_POLY1305_C
#endif
#if defined(MBEDTLS_CONFIG_CRYPTO_CHACHAPOLY)
#define MBEDTLS_CHACHAPOLY_C
#endif
#if defined(MBEDTLS_CONFIG_CRYPTO_DES)
#define MBEDTLS_DES_C
#endif
#if defined(MBEDTLS_CONFIG_CRYPTO_XTEA)
#define MBEDTLS_XTEA_C
#endif

#if defined(MBEDTLS_CONFIG_TLS_DEBUG)
#define MBEDTLS_DEBUG_C
#endif
#if defined(MBEDTLS_CONFIG_ERROR)
#define MBEDTLS_ERROR_C
#endif

#if defined(MBEDTLS_CONFIG_CRYPTO_MD5)
#define MBEDTLS_MD5_C
#endif
#if defined(MBEDTLS_CONFIG_CRYPTO_RIPEMD160)
#define MBEDTLS_RIPEMD160_C
#endif
#if defined(MBEDTLS_CONFIG_CRYPTO_SHA1)
#define MBEDTLS_SHA1_C
#endif
#if defined(MBEDTLS_CONFIG_CRYPTO_SHA256)
#define MBEDTLS_SHA256_C
#endif
#if defined(MBEDTLS_CONFIG_CRYPTO_SHA512)
#define MBEDTLS_SHA512_C
#endif

#if defined(MBEDTLS_CONFIG_TLS)
#define MBEDTLS_SSL_CACHE_C
#define MBEDTLS_SSL_COOKIE_C
#define MBEDTLS_SSL_CLI_C
#define MBEDTLS_SSL_TLS_C
#endif

#if defined(MBEDTLS_CONFIG_DTLS)
#define MBEDTLS_SSL_SRV_C
#endif
#if defined(MBEDTLS_CONFIG_CRYPTO_AES)
#define MBEDTLS_CTR_DRBG_C
#endif
#define MBEDTLS_KEY_EXCHANGE_PSK_ENABLED
#define MBEDTLS_KEY_EXCHANGE__SOME__PSK_ENABLED

#define MBEDTLS_ENTROPY_C

#if defined(MBEDTLS_CONFIG_X509)
#define MBEDTLS_X509_USE_C
#define MBEDTLS_X509_CRT_PARSE_C
#define MBEDTLS_X509_CRL_PARSE_C
#endif

/* Module configuration options */
#if defined(MBEDTLS_CONFIG_TLS)
#define MBEDTLS_SSL_MAX_CONTENT_LEN          (16384)
#define MBEDTLS_TLS_DEFAULT_ALLOW_SHA1_IN_KEY_EXCHANGE
#endif

/*
 * User custom configuration file to add or override the above default
 * configurations
 */
#if defined(MBEDTLS_CONFIG_USER_FILE)
#include MBEDTLS_CONFIG_USER_FILE
#endif

#include "mbedtls/check_config.h"

#endif /* MBEDTLS_CONFIG_H */
