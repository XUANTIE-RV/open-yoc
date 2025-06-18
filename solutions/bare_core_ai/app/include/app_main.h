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

#ifndef __APP_MAIN_H__
#define __APP_MAIN_H__

#include <string.h>

#define TYPE 32
int add_main(int argc, char **argv);
int exp_main(int argc, char **argv);
int gather_main(int argc, char **argv);
int layer_norm_main(int argc, char **argv);
int matmul_main(int argc, char **argv);
int mul_main(int argc, char **argv);
int slice_main(int argc, char **argv);
int softmax_main(int argc, char **argv);
int sub_main(int argc, char **argv);
int where_main(int argc, char **argv);

void *fastmalloc(size_t size);
void fastfree(void *ptr);
void *align_fast_malloc(size_t size);
void align_free(void *ptr);

#endif