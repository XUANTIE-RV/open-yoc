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

L_MODULE := libksal

L_CFLAGS := -Wall

L_INCS +=

#L_SRCS += ip4_addr.c
L_SRCS += sal.c
#L_SRCS += sal_arch.c
#L_SRCS += sal_err.c
L_SRCS += sal_sockets.c
L_SRCS += ../lwip/core/def.c
L_SRCS += ../lwip/core/ipv4/ip4_addr.c
L_SRCS += ../lwip/api/err.c
#L_SRCS += ../lwip/port/sys_arch.c
L_SRCS += apps/ping.c

ifeq ($(CONFIG_SAL), y)
include $(BUILD_MODULE)
endif
