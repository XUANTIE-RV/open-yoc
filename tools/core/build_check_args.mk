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

comma := ,
ifneq ($(cpu),)
CPU_FULL_MACRO_NAME := $(shell bash $(TOP_DIR)/tools/core/cpu2fullname.sh $(cpu))
CPU_FULL_NAME 	:= $(word 1,$(subst $(comma), ,$(CPU_FULL_MACRO_NAME)))
CPU_MACRO 		:= $(word 2,$(subst $(comma), ,$(CPU_FULL_MACRO_NAME)))
CPU 		:= $(cpu)
BOARD 		:= $(board)
RTOS 		:= $(rtos)
TOOLCHAIN 	:= $(toolchain)

$(info [info]: CPU_NAME:      $(CPU))
$(info [info]: CPU_FULL_NAME: $(CPU_FULL_NAME))
$(info [info]: CPU_MACRO:     $(CPU_MACRO))
ifeq ($(CPU_FULL_NAME),)
$(info [info]: There is no CPU_FULL_NAME for cpu '$(cpu)',please check the cpu argument.)
exit 1
endif
ifeq ($(CPU_MACRO),)
$(info [info]: There is no CPU_MACRO for cpu '$(cpu)',please check the cpu argument.)
exit 1
endif
endif

ifeq ($(board), smartl)
else ifeq ($(board), xiaohui)
else ifeq ($(board), wujian300)
else
$(info [info]: Can't found board '$(board)',please check the board argument.)
exit 1
endif
