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

CUR_COMP_NAME := aos
CUR_COMP_DIR := $(TOP_DIR)/components/$(CUR_COMP_NAME)
# $(info come to $(CUR_COMP_DIR))

ifeq ($(COLLECT_BUILD_FILES), 0)
CFLAGS += -DCONFIG_OPT_FOOTPRINT_LEVEL=0 -DCONFIG_INIT_TASK_STACK_SIZE=8192 -DCONFIG_AOS_NEWLINE_SUPPORT=1 -DCONFIG_DEVICEID_FROM_KV=1 \
		  -DCONFIG_AOS_OSAL=1 -DAOS_RTT_SPIN_LOCK_STRUCT_SIZE=32
CFLAGS += -Wno-implicit-exception-spec-mismatch
else
$(CUR_COMP_NAME)_incs := $(CUR_COMP_DIR)/include

$(CUR_COMP_NAME)_c_srcs := $(CUR_COMP_DIR)/src/crc16.c \
                           $(CUR_COMP_DIR)/src/except.c \
                           $(CUR_COMP_DIR)/src/list.c \
                           $(CUR_COMP_DIR)/src/hash.c \
                           $(CUR_COMP_DIR)/src/lpm.c \
                           $(CUR_COMP_DIR)/src/main.c \
                           $(CUR_COMP_DIR)/src/ringbuffer.c \
                           $(CUR_COMP_DIR)/src/select.c \
                           $(CUR_COMP_DIR)/src/settings_ali.c \
                           $(CUR_COMP_DIR)/src/softwdt.c \
                           $(CUR_COMP_DIR)/src/sysinfo.c \
                           $(CUR_COMP_DIR)/src/ustdio.c \
                           $(CUR_COMP_DIR)/src/aos_porting.c \
                           $(CUR_COMP_DIR)/src/console_uart.c \
                           $(wildcard $(CUR_COMP_DIR)/src/eventloop/*.c) \
                           $(wildcard $(CUR_COMP_DIR)/src/cplusplus/*.c)

ifeq ($(call check_cflag_exact_func, -DCONFIG_KERNEL_RHINO=1), 1)
$(CUR_COMP_NAME)_c_srcs += $(wildcard $(CUR_COMP_DIR)/src/adapter/rhino/*.c)
endif
ifeq ($(call check_cflag_exact_func, -DCONFIG_KERNEL_FREERTOS=1), 1)
$(CUR_COMP_NAME)_c_srcs += $(wildcard $(CUR_COMP_DIR)/src/adapter/freertos/*.c)
endif
ifeq ($(call check_cflag_exact_func, -DCONFIG_KERNEL_RTTHREAD=1), 1)
$(CUR_COMP_NAME)_c_srcs += $(wildcard $(CUR_COMP_DIR)/src/adapter/rtthread/*.c)
endif

$(CUR_COMP_NAME)_cpp_srcs := $(wildcard $(CUR_COMP_DIR)/src/cplusplus/*.cpp)
endif
