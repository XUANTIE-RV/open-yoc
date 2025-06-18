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

CUR_COMP_NAME := freertos
CUR_COMP_DIR := $(TOP_DIR)/components/$(CUR_COMP_NAME)
# $(info come to $(CUR_COMP_DIR))

ifeq ($(COLLECT_BUILD_FILES), 0)
CFLAGS += -DCONFIG_KERNEL_FREERTOS=1
else
$(CUR_COMP_NAME)_incs := $(CUR_COMP_DIR)/FreeRTOS/Source/include \
                         $(CUR_COMP_DIR)/FreeRTOS/Source/portable/GCC/$(ARCH)/$(CPU_FULL_NAME) \
                         $(CUR_COMP_DIR)/FreeRTOS/Source/portable/GCC/$(ARCH)/$(CPU_FULL_NAME)/tspend \

$(CUR_COMP_NAME)_c_srcs := $(CUR_COMP_DIR)/FreeRTOS/Source/timers.c \
                           $(CUR_COMP_DIR)/FreeRTOS/Source/tasks.c \
                           $(CUR_COMP_DIR)/FreeRTOS/Source/queue.c \
                           $(CUR_COMP_DIR)/FreeRTOS/Source/list.c \
                           $(CUR_COMP_DIR)/FreeRTOS/Source/event_groups.c \
                           $(CUR_COMP_DIR)/FreeRTOS/Source/portable/MemMang/heap_4.c \
                           $(CUR_COMP_DIR)/FreeRTOS/Source/portable/GCC/$(ARCH)/$(CPU_FULL_NAME)/port.c \
                           $(CUR_COMP_DIR)/freertos_native.c

$(CUR_COMP_NAME)_asm_srcs := $(CUR_COMP_DIR)/FreeRTOS/Source/portable/GCC/$(ARCH)/$(CPU_FULL_NAME)/tspend/cpu_task_sw.S
endif
