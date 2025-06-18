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

CUR_COMP_NAME := cli
CUR_COMP_DIR := $(TOP_DIR)/components/$(CUR_COMP_NAME)
# $(info come to $(CUR_COMP_DIR))

ifeq ($(COLLECT_BUILD_FILES), 0)
CFLAGS += -DAOS_COMP_CLI=1 -DCLI_INBUF_SIZE=256 -DCLI_OUTBUF_SIZE=512 -DCLI_MAX_COMMANDS=128 -DCLI_MAX_ARG_NUM=16 \
          -DCLI_MAX_ONCECMD_NUM=4 -DCLI_TASK_PRIORITY=60 -DCLI_CONFIG_STACK_SIZE=2048 -DCLI_TELNET_ENABLE=0 \
          -DCLI_UAGENT_ENABLE=0 -DCLI_SEPRATED_CONSOLE=0 -DCLI_IOBOX_ENABLE=0 -DCONFIG_LFS_MOUNTPOINT="/"
else
$(CUR_COMP_NAME)_incs := $(CUR_COMP_DIR)/include \
                         $(CUR_COMP_DIR)/src $(CUR_COMP_DIR)/src/iobox

$(CUR_COMP_NAME)_c_srcs := $(CUR_COMP_DIR)/src/cli.c \
                           $(CUR_COMP_DIR)/src/cli_console.c \
                           $(CUR_COMP_DIR)/src/cli_adapt.c \
                           $(CUR_COMP_DIR)/src/cli_aos.c \
                           $(CUR_COMP_DIR)/src/uart_console.c \
                           $(wildcard $(CUR_COMP_DIR)/src/sys/*.c)

ifeq ($(call check_cflag_exact_func, -DCLI_TELNET_ENABLE=1), 1)
$(CUR_COMP_NAME)_c_srcs += $(CUR_COMP_DIR)/src/telnet_console.c
endif
ifeq ($(call check_cflag_exact_func, -DCLI_UAGENT_ENABLE=1), 1)
$(CUR_COMP_NAME)_c_srcs += $(CUR_COMP_DIR)/src/cli_uagent.c
endif
ifeq ($(call check_cflag_exact_func, -DCLI_IOBOX_ENABLE=1), 1)
$(CUR_COMP_NAME)_c_srcs += $(CUR_COMP_DIR)/src/iobox/cat.c \
                           $(CUR_COMP_DIR)/src/iobox/cd.c \
                           $(CUR_COMP_DIR)/src/iobox/cp.c \
                           $(CUR_COMP_DIR)/src/iobox/df.c \
                           $(CUR_COMP_DIR)/src/iobox/echo.c \
                           $(CUR_COMP_DIR)/src/iobox/ls.c \
                           $(CUR_COMP_DIR)/src/iobox/lsfs.c \
                           $(CUR_COMP_DIR)/src/iobox/mkdir.c \
                           $(CUR_COMP_DIR)/src/iobox/mv.c \
                           $(CUR_COMP_DIR)/src/iobox/rm.c \
                           $(CUR_COMP_DIR)/src/iobox/touch.c \
                           $(CUR_COMP_DIR)/src/iobox/pwd.c \
                           $(CUR_COMP_DIR)/src/iobox/hexdump.c \
                           $(CUR_COMP_DIR)/src/iobox/path_helper.c \
                           $(CUR_COMP_DIR)/src/iobox/rwspeed.c
endif
endif
