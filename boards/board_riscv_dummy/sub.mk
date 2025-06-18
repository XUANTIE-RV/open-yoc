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

CUR_COMP_NAME := board_riscv_dummy
CUR_COMP_DIR := $(TOP_DIR)/boards/$(CUR_COMP_NAME)
# $(info come to $(CUR_COMP_DIR))

ifeq ($(COLLECT_BUILD_FILES), 0)
ifeq ($(board), xiaohui)
CFLAGS += -DCONFIG_INIT_TASK_STACK_SIZE=8192 -DCONFIG_APP_TASK_STACK_SIZE=8192 -DCONFIG_SYSTICK_HZ=100 -DCONFIG_BOARD_XIAOHUI_EVB=1
endif
ifeq ($(board), smartl)
CFLAGS += -DCONFIG_INIT_TASK_STACK_SIZE=4096 -DCONFIG_APP_TASK_STACK_SIZE=8192 -DCONFIG_SYSTICK_HZ=100 -DCONFIG_BOARD_SMARTL_EVB=1
endif
else
$(CUR_COMP_NAME)_incs := $(CUR_COMP_DIR)/include

$(CUR_COMP_NAME)_c_srcs := 	$(CUR_COMP_DIR)/src/board_init.c \
                            $(CUR_COMP_DIR)/src/uart/board_uart.c
endif
