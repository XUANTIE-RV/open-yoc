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

CUR_COMP_NAME := ulog
CUR_COMP_DIR := $(TOP_DIR)/components/$(CUR_COMP_NAME)
# $(info come to $(CUR_COMP_DIR))

ifeq ($(COLLECT_BUILD_FILES), 0)
CFLAGS += -DAOS_COMP_ULOG=y -DULOG_TAG_USE_SHORT_FILE=1 -DULOG_CONFIG_POP_FS_PATH="/fatfs/"
else
$(CUR_COMP_NAME)_incs := $(CUR_COMP_DIR)/include \
                         $(CUR_COMP_DIR)/internal

$(CUR_COMP_NAME)_c_srcs := $(CUR_COMP_DIR)/src/ulog.c \
                           $(CUR_COMP_DIR)/src/ulog_cli.c \
                           $(CUR_COMP_DIR)/src/ulog_init.c \
                           $(CUR_COMP_DIR)/src/ulog_utility.c
#async mode actived
ifeq ($(call check_cflag_exact_func, -DULOG_CONFIG_ASYNC=1), 1)
$(CUR_COMP_NAME)_c_srcs += $(CUR_COMP_DIR)/src/ulog_async.c $(CUR_COMP_DIR)/src/ulog_ring_fifo.c
endif

#ulog support fs record
ifeq ($(call check_cflag_exact_func, -DULOG_CONFIG_POP_FS=1), 1)
$(CUR_COMP_NAME)_c_srcs += $(CUR_COMP_DIR)/src/ulog_session_file.c $(CUR_COMP_DIR)/src/ulog_fs_cfg.c
endif #ULOG_CONFIG_POP_FS
endif
