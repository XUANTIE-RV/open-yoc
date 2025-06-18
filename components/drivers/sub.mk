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

CUR_COMP_NAME := drivers
CUR_COMP_DIR := $(TOP_DIR)/components/$(CUR_COMP_NAME)
# $(info come to $(CUR_COMP_DIR))

ifeq ($(COLLECT_BUILD_FILES), 0)
CFLAGS += -DCONFIG_UART_RECV_BUF_SIZE=1024
else
$(CUR_COMP_NAME)_incs := $(CUR_COMP_DIR)/csi2/include

$(CUR_COMP_NAME)_c_srcs := $(wildcard $(CUR_COMP_DIR)/csi2/iic/*.c) \
                           $(wildcard $(CUR_COMP_DIR)/csi2/uart/*.c) \
                           $(wildcard $(CUR_COMP_DIR)/csi2/spiflash/*.c) \
                           $(wildcard $(CUR_COMP_DIR)/csi2/adc/*.c) \
                           $(wildcard $(CUR_COMP_DIR)/csi2/rtc/*.c) \
                           $(wildcard $(CUR_COMP_DIR)/csi2/wdt/*.c) \
                           $(wildcard $(CUR_COMP_DIR)/csi2/gpio/*.c) \
                           $(wildcard $(CUR_COMP_DIR)/csi2/clk/*.c) \
                           $(wildcard $(CUR_COMP_DIR)/csi2/sdmmc/*.c) \
                           $(wildcard $(CUR_COMP_DIR)/csi2/i2s/*.c) \
                           $(wildcard $(CUR_COMP_DIR)/csi2/pwm/*.c) \
                           $(wildcard $(CUR_COMP_DIR)/csi2/spi/*.c) \
                           $(wildcard $(CUR_COMP_DIR)/csi2/timer/*.c)
endif
