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
#ifndef SC_MBEDTLS_CONFIG_H
#define SC_MBEDTLS_CONFIG_H

/* System support */


/* mbed TLS feature support */
#define MBEDTLS_PKCS1_V15


/* mbed TLS modules */
#define MBEDTLS_AES_C
#define MBEDTLS_CIPHER_MODE_CBC
#define MBEDTLS_ASN1_PARSE_C
#define MBEDTLS_BIGNUM_C

#define SC_MBEDTLS_MD_C

//#define MBEDTLS_NET_C
#define MBEDTLS_OID_C
#define MBEDTLS_SHA1_C
#define MBEDTLS_SHA256_C
#define MBEDTLS_RSA_C
#define MBEDTLS_RSA_NO_CRT


#if !defined (MBEDTLS_DEBUG_C)
/*reduce readonly date size*/
#define CK_REMOVE_UNUSED_FUNCTION_AND_DATA
#endif

#endif /* SC_MBEDTLS_CONFIG_H */
