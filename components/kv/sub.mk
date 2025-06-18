##
 # Copyright (C) 2017-2024 Alibaba Group Holding Limited
 #
 # SPDX-License-Identifier: Apache-2.0
 #
 # Licensed under the Apache License, Version 2.0 (the "License");
 # you may not use this file except in compliance with the License.
 # You may obtain a copy of the License at
 #
 #     http://www.apache.org/licenses/LICENSE-2.0
 #
 # Unless required by applicable law or agreed to in writing, software
 # distributed under the License is distributed on an "AS IS" BASIS,
 # WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 # See the License for the specific language governing permissions and
 # limitations under the License.
##

CUR_COMP_NAME := kv
CUR_COMP_DIR := $(TOP_DIR)/components/$(CUR_COMP_NAME)
# $(info come to $(CUR_COMP_DIR))

ifeq ($(COLLECT_BUILD_FILES), 0)
CFLAGS += -DCONFIG_KV_ENABLE_CACHE=0 -DCONFIG_KV_START_OPT=1 -DCONFIG_KV_HASH_BUCKET=16 -DCONFIG_KV_GET_ERASE_FLAG_AUTO=0 -DCONFIG_KV_LARGE_NODE=0
else
$(CUR_COMP_NAME)_incs := $(CUR_COMP_DIR)/include

$(CUR_COMP_NAME)_c_srcs := $(CUR_COMP_DIR)/block.c \
                           $(CUR_COMP_DIR)/kvset.c \
                           $(CUR_COMP_DIR)/kv_cache.c \
                           $(CUR_COMP_DIR)/kv_aos.c \
                           $(CUR_COMP_DIR)/kv_fct.c

ifeq ($(call check_cflag_exact_func, -DAOS_COMP_CLI=1), 1)
$(CUR_COMP_NAME)_c_srcs += $(CUR_COMP_DIR)/cli_kvtool.c
endif
endif
