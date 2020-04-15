
# Ropyright (C) 2017 C-SKY Microsystems Co., All rights reserved.
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

L_LWIP_PATH := $(L_PATH)

include $(DEFINE_LOCAL)

L_MODULE := libklwip

L_CFLAGS := -Wno-address -Wno-unused-but-set-variable

L_INCS :=

LWIPDIR :=
include $(L_LWIP_PATH)/Filelists.mk
L_INCS += $(L_LWIP_PATH)/include

L_SRCS += $(DHCPSFILES)
L_SRCS += $(COREFILES)
L_SRCS += $(CORE4FILES)
L_SRCS += $(CORE6FILES)
L_SRCS += $(APIFILES)
L_SRCS += $(NETIFFILES)
L_SRCS += $(PINGFILES)

#mv to kernel to let select standalone
#L_SRCS += port/sys_arch.c

#L_SRCS += apps/ping.c

ifeq ($(CONFIG_TCPIP), y)
include $(BUILD_MODULE)
endif

