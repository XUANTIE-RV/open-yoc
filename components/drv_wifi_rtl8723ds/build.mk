##
 # Copyright (C) 2017 C-SKY Microsystems Co., All rights reserved.
 #
 # Licensed under the Apache License, Version 2.0 (the "License");
 # you may not use this file except in compliance with the License.
 # You may obtain a copy of the License at
 #
 #   http://www.apache.org/licenses/LICENSE-2.0
 #
 # Unless required by applicable law or agreed to in writing, software
 # distributed under the License is distributed on an "AS IS" BASIS,
 # WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 # See the License for the specific language governing permissions and
 # limitations under the License.
##

L_PATH := $(call cur-dir)

include $(DEFINE_LOCAL)

#L_MODULE := librtl8723ds

#L_CFLAGS += -Wall

L_INCS += $(L_PATH) \
		$(L_PATH)/api \
		$(L_PATH)/api/wifi \
		$(L_PATH)/include \
		$(L_PATH)/os/customer_rtos \
		$(L_PATH)/os/os_dep/include \
		$(L_PATH)/platform/ \
		$(L_PATH)/platform/include \
		$(L_PATH)/platform/rtwlan_bsp \
		$(L_PATH)/platform/sdio/core \
		$(L_PATH)/platform/sdio/include

L_SRCS += api/wifi/wifi_conf.c
L_SRCS += api/wifi/wifi_ind.c
L_SRCS += api/wifi/wifi_util.c

L_PRE_LIBS += $(shell echo $(CONFIG_CHIP_CPU)/wlan_lib.a)

#include $(BUILD_MODULE)