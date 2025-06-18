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

CUR_COMP_NAME := debug
CUR_COMP_DIR := $(TOP_DIR)/components/$(CUR_COMP_NAME)
# $(info come to $(CUR_COMP_DIR))

ifeq ($(COLLECT_BUILD_FILES), 0)
CFLAGS += -DAOS_COMP_DEBUG=1 -DDEBUG_PANIC_PRT_INT=1
# cpuusage feature config
CFLAGS += -DDEBUG_CPUUSAGE_RECODE_TO_FILE_ENABLE=0 -DDEBUG_CPUUSAGE_MAX_TASK=80 -DDEBUG_CPUUSAGE_FILE_NAME="/data/cpuusage"
# last_word feature config
CFLAGS += -DDEBUG_LAST_WORD_ENABLE=0 -DDEBUG_LOG_FILE_NUM=10 -DDEBUG_LASTWORD_RAM_ADDR=0 -DDEBUG_LASTWORD_REGION_LEN=0x4000 \
          -DDEBUG_LOG_DIR_NAME="/data/crash_reports" -DDEBUG_CONFIG_LOG_FILE_NAME="/data/crash_reports/crash_report" \
          -DDEBUG_CONFIG_LOG_FILE_INDEX="/data/crash_reports/log_file_index"
# ulog flush
CFLAGS += -DDEBUG_ULOG_FLUSH=0
# debug cmds
CFLAGS += -DDEBUG_TEST_CMDS_ENABLE=0
else
$(CUR_COMP_NAME)_incs := $(CUR_COMP_DIR)/include \
                         $(CUR_COMP_DIR)/include/debug \
                         $(CUR_COMP_DIR)/src/arch

$(CUR_COMP_NAME)_c_srcs := $(CUR_COMP_DIR)/src/debug_panic.c \
                           $(CUR_COMP_DIR)/src/debug_print.c \
                           $(CUR_COMP_DIR)/src/debug_dumpsys.c \
                           $(CUR_COMP_DIR)/src/debug_cli_cmd.c \
                           $(CUR_COMP_DIR)/src/debug.c \
                           $(CUR_COMP_DIR)/src/debug_weak.c

ifeq ($(call check_cflag_exact_func, -DCONFIG_KERNEL_RHINO=1), 1)
$(CUR_COMP_NAME)_c_srcs += $(CUR_COMP_DIR)/src/debug_overview.c \
                           $(CUR_COMP_DIR)/src/debug_backtrace.c \
                           $(CUR_COMP_DIR)/src/debug_infoget.c \
                           $(CUR_COMP_DIR)/src/debug_cpuusage.c
endif
ifeq ($(call check_cflag_exact_func, -DDEBUG_TEST_CMDS_ENABLE=1), 1)
$(CUR_COMP_NAME)_c_srcs += $(CUR_COMP_DIR)/src/debug_test.c
endif
ifeq ($(call check_cflag_exact_func, -DDEBUG_LAST_WORD_ENABLE=1), 1)
$(CUR_COMP_NAME)_c_srcs += $(CUR_COMP_DIR)/src/debug_lastword.c
endif
endif
