/**
 * \file config.h
 *
 * \brief Configuration options (set of defines)
 *
 *  This set of compile-time options may be used to enable
 *  or disable features selectively, and reduce the global
 *  memory footprint.
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

#ifndef __BLCRYPTO_SUITE_BIGNUM_MULTIPLEX_H__
#define __BLCRYPTO_SUITE_BIGNUM_MULTIPLEX_H__

#define blcrypto_suite_mpi_init              mbedtls_mpi_init
#define blcrypto_suite_mpi_free              mbedtls_mpi_free
#define blcrypto_suite_mpi_grow              mbedtls_mpi_grow
#define blcrypto_suite_mpi_shrink            mbedtls_mpi_shrink
#define blcrypto_suite_mpi_copy              mbedtls_mpi_copy
#define blcrypto_suite_mpi_swap              mbedtls_mpi_swap
#define blcrypto_suite_mpi_safe_cond_assign  mbedtls_mpi_safe_cond_assign
#define blcrypto_suite_mpi_safe_cond_swap    mbedtls_mpi_safe_cond_swap
#define blcrypto_suite_mpi_lset              mbedtls_mpi_lset
#define blcrypto_suite_mpi_get_bit           mbedtls_mpi_get_bit
#define blcrypto_suite_mpi_set_bit           mbedtls_mpi_set_bit
#define blcrypto_suite_mpi_lsb               mbedtls_mpi_lsb
#define blcrypto_suite_mpi_bitlen            mbedtls_mpi_bitlen
#define blcrypto_suite_mpi_size              mbedtls_mpi_size
#define blcrypto_suite_mpi_read_string       mbedtls_mpi_read_string
#define blcrypto_suite_mpi_write_string      mbedtls_mpi_write_string     

#if defined(BLCRYPTO_SUITE_FS_IO)
#define blcrypto_suite_mpi_read_file         mbedtls_mpi_read_file
#define blcrypto_suite_mpi_write_file        mbedtls_mpi_write_file
#endif /* BLCRYPTO_SUITE_FS_IO */

#define blcrypto_suite_mpi_read_binary       mbedtls_mpi_read_binary  
#define blcrypto_suite_mpi_write_binary      mbedtls_mpi_write_binary
#define blcrypto_suite_mpi_shift_l           mbedtls_mpi_shift_l
#define blcrypto_suite_mpi_shift_r           mbedtls_mpi_shift_r
#define blcrypto_suite_mpi_cmp_abs           mbedtls_mpi_cmp_abs
#define blcrypto_suite_mpi_cmp_mpi           mbedtls_mpi_cmp_mpi
#define blcrypto_suite_mpi_lt_mpi_ct         mbedtls_mpi_lt_mpi_ct
#define blcrypto_suite_mpi_cmp_int           mbedtls_mpi_cmp_int
#define blcrypto_suite_mpi_add_abs           mbedtls_mpi_add_abs
#define blcrypto_suite_mpi_sub_abs           mbedtls_mpi_sub_abs
#define blcrypto_suite_mpi_add_mpi           mbedtls_mpi_add_mpi
#define blcrypto_suite_mpi_sub_mpi           mbedtls_mpi_sub_mpi
#define blcrypto_suite_mpi_add_int           mbedtls_mpi_add_int
#define blcrypto_suite_mpi_sub_int           mbedtls_mpi_sub_int
#define blcrypto_suite_mpi_mul_mpi           mbedtls_mpi_mul_mpi
#define blcrypto_suite_mpi_mul_int           mbedtls_mpi_mul_int
#define blcrypto_suite_mpi_div_mpi           mbedtls_mpi_div_mpi
#define blcrypto_suite_mpi_div_int           mbedtls_mpi_div_int
#define blcrypto_suite_mpi_mod_mpi           mbedtls_mpi_mod_mpi
#define blcrypto_suite_mpi_mod_int           mbedtls_mpi_mod_int
#define blcrypto_suite_mpi_exp_mod           mbedtls_mpi_exp_mod
#define blcrypto_suite_mpi_fill_random       mbedtls_mpi_fill_random
#define blcrypto_suite_mpi_gcd               mbedtls_mpi_gcd
#define blcrypto_suite_mpi_inv_mod           mbedtls_mpi_inv_mod
#define blcrypto_suite_mpi_is_prime          mbedtls_mpi_is_prime
#define blcrypto_suite_mpi_is_prime_ext      mbedtls_mpi_is_prime_ext
#define blcrypto_suite_mpi_gen_prime         mbedtls_mpi_gen_prime   

#endif /* BLCRYPTO_SUITE_CONFIG_H */
