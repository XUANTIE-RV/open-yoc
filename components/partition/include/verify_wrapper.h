 /*
 * Copyright (C) 2017-2024 Alibaba Group Holding Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef __VERIFY_WRAPPER_H__
#define __VERIFY_WRAPPER_H__

#include <stdint.h>
#include "verify.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
  * @param[in]   ds         digest_sch_e
  * @param[in]   input      input buf. if from flash, use Absolute Address
  * @param[in]   ilen       input len
  * @param[out]  output     output data buf
  * @param[out]  olen       output length
  * @param[in]   from_mem   the input data source, 0：flash， 1：ram
  * @param[in]   part_info  the input data's partition information
  *
  * @return  0: On success， otherwise is error
*/
/* TODO weak */
int hash_calc_start(digest_sch_e ds, const unsigned char *input, int ilen,
                    unsigned char *output, uint32_t *olen,
                    int from_mem, partition_info_t *part_info);

/*
  * @param[in]   ds         digest_sch_e
  * @param[in]   ss         signature_sch_e
  * @param[in]   key        key buffer
  * @param[in]   key_size   key size
  * @param[in]   src        pointer to the source data.
  * @param[in]   src_size   the source data length
  * @param[in]   signature  pointer to the signature
  * @param[in]   sig_size   the signature size
  *
  * @return  0: On success， otherwise is error
*/
/* TODO weak */
int signature_verify_start(digest_sch_e ds, signature_sch_e ss,
                            uint8_t *key_buf, int key_size,
                            const uint8_t *src, int src_size,
                            const uint8_t *signature, int sig_size);

/*
  * @param[in]   input      pointer to the source data.
  * @param[in]   ilen       the source data length
  * @param[in]   output     the crc32 value
  *
  * @return  0: On success， otherwise is error
*/
/* TODO weak */
int crc32_calc_start(const uint8_t *input, uint32_t ilen, uint32_t *output);

#ifdef __cplusplus
}
#endif
#endif